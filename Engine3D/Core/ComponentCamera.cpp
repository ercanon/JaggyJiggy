#include "ComponentCamera.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include "ComponentTransform.h"
#include "SDL/include/SDL_opengl.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent) {}	

ComponentCamera::ComponentCamera(GameObject* parent, bool CanCam) : Component(parent)
{
	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 5.0f, -30.0f);
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

	App->viewportBuffer2->PreUpdate(App->dt);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix.Transposed().ptr());
}
