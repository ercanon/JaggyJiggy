#include "ComponentCamera.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ComponentTransform.h"
#include "SDL/include/SDL_opengl.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent) {}	

ComponentCamera::ComponentCamera(GameObject* parent, bool CanCam) : Component(parent)
{
	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 20.0f, -30.0f);
	owner->transform->SetPosition(position);
	reference = float3(0.0f, 0.0f, 0.0f);

	CalculateViewMatrix();

	LookAt(float3::zero);
}

ComponentCamera::~ComponentCamera() {}

void ComponentCamera::LookAt(const float3& point) {
	reference = point;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}

bool ComponentCamera::Update(float dt)
{
	if (isMouseFocusedGame)
	{
		if (!ImGuizmo::IsUsing())
		{
			float3 newPos(0, 0, 0);
			float speed = cameraSpeed * dt;
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
				speed *= 4.f;

			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
				newPos.y += speed;
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
				newPos.y -= speed;

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
				newPos += front * speed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
				newPos -= front * speed;


			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
				newPos += right * speed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
				newPos -= right * speed;

			if (App->input->GetMouseZ() > 0)
				newPos += front * speed * 2;
			if (App->input->GetMouseZ() < 0)
				newPos -= front * speed * 2;

			position += newPos;
			owner->transform->SetPosition(position);

			// Recalculate matrix -------------
			if (!newPos.Equals(float3::zero)) CalculateViewMatrix();
		}

		// Mouse motion ----------------
		bool hasRotated = false;

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {

				const float newDeltaX = (float)dx * cameraSensitivity;
				const float newDeltaY = (float)dy * cameraSensitivity;

				reference = owner->transform->GetPosition();
				Quat orbitMat = Quat::RotateY(newDeltaX * .1f);

				if (abs(up.y) < 0.3f) // Avoid gimball lock on up & down apex
				{
					if (position.y > reference.y && newDeltaY < 0.f)
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);
					if (position.y < reference.y && newDeltaY > 0.f)
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);
				}
				else
				{
					orbitMat = orbitMat * math::Quat::RotateAxisAngle(right, newDeltaY * .1f);
				}

				position = orbitMat * (position - reference) + reference;

				CalculateViewMatrix();
				LookAt(reference);
			}
			else
			{
				if (dx != 0)
				{
					const float newDeltaX = (float)dx * cameraSensitivity;
					float deltaX = newDeltaX + 0.95f * (lastDeltaX - newDeltaX); //lerp for smooth rotation acceleration to avoid jittering
					lastDeltaX = deltaX;
					Quat rotateY = Quat::RotateY(up.y >= 0.f ? deltaX * .1f : -deltaX * .1f);
					up = rotateY * up;
					front = rotateY * front;
					CalculateViewMatrix();
					hasRotated = true;
				}

				if (dy != 0)
				{
					const float newDeltaY = (float)dy * cameraSensitivity;
					float deltaY = newDeltaY + 0.95f * (lastDeltaY - newDeltaY); //lerp for smooth rotation acceleration to avoid jittering
					lastDeltaY = deltaY;
					Quat rotateX = Quat::RotateAxisAngle(right, -deltaY * .1f);
					up = rotateX * up;
					front = rotateX * front;
					CalculateViewMatrix();
					hasRotated = true;
				}
			}
		}

		!hasRotated ? lastDeltaX = lastDeltaY = 0.f : 0.f;
	}

	return true;
}

void ComponentCamera::CalculateViewMatrix() {
	if (projectionIsDirty)
		RecalculateProjection();

	cameraFrustum.pos = position;
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void ComponentCamera::RecalculateProjection() {
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

void ComponentCamera::OnGui() {

	if (ImGui::CollapsingHeader("Camera Settings"))
	{
		ImGui::DragFloat("Near Distance", &cameraFrustum.nearPlaneDistance);
		ImGui::DragFloat("Far Distance", &cameraFrustum.farPlaneDistance);
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) RecalculateProjection();
	}

	if (cameraFrustum.nearPlaneDistance < 1) cameraFrustum.nearPlaneDistance = 1;
	else if (cameraFrustum.nearPlaneDistance >= cameraFrustum.farPlaneDistance) 
		cameraFrustum.nearPlaneDistance = cameraFrustum.farPlaneDistance - 30;

	if (cameraFrustum.farPlaneDistance < 30) cameraFrustum.farPlaneDistance = 30;
	else if (cameraFrustum.farPlaneDistance <= cameraFrustum.nearPlaneDistance)
		cameraFrustum.farPlaneDistance = cameraFrustum.nearPlaneDistance + 30;

	if (verticalFOV <= 1) verticalFOV = 1;
	else if (verticalFOV >= 359) verticalFOV = 359;
}

void ComponentCamera::Draw() {
	position = owner->transform->GetPosition();
	up = owner->transform->GetUp();
	front = owner->transform->GetFront();

	CalculateViewMatrix();

	App->viewportBufferGame->PreUpdate(App->dt);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix.Transposed().ptr());
}
