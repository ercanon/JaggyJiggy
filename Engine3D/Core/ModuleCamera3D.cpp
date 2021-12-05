#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Geometry/AABB.h"
#include <vector>
#include <map>

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 5.0f, -15.0f);
	reference = float3(0.0f, 0.0f, 0.0f);
	
	CalculateViewMatrix();

}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");

	LookAt(float3::zero);

	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	if (isMouseFocused)
	{
		float3 newPos(0, 0, 0);
		float speed = cameraSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed *= 4.f;

		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
			newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
			newPos.y -= speed;

		//Focus
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			if (App->editor->gameobjectSelected != nullptr)
			{
				if (ComponentMesh* mesh = App->editor->gameobjectSelected->GetComponent<ComponentMesh>())
				{
					const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
					LookAt(meshCenter);
					const float meshRadius = mesh->GetSphereRadius();
					const float currentDistance = meshCenter.Distance(position);
					const float desiredDistance = (meshRadius * 2) / atan(cameraFrustum.horizontalFov);
					position = position + front * (currentDistance - desiredDistance);
				}
				else
				{
					LookAt(App->editor->gameobjectSelected->transform->GetPosition());
				}
			}
		}

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

		// Recalculate matrix -------------
		if (!newPos.Equals(float3::zero)) CalculateViewMatrix();

		// Mouse motion ----------------

		bool hasRotated = false;

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) {
				if (App->editor->gameobjectSelected != nullptr)
				{
					const float newDeltaX = (float)dx * cameraSensitivity;
					const float newDeltaY = (float)dy * cameraSensitivity;

					reference = App->editor->gameobjectSelected->transform->GetPosition();
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

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::IsMouseClicked()
{
	float positionX, positionY, normalX, normalY;
	
	positionX = ImGui::GetMousePos().x;
	positionY = ImGui::GetMousePos().y;

	normalX = (((positionX - ImGui::GetWindowPos().x) / 
		((ImGui::GetWindowPos().x + ImGui::GetWindowSize().x) - ImGui::GetWindowPos().x)) - 0.5f) / 0.5f;
	normalY = -((((positionY - (ImGui::GetWindowPos().y + ImGui::GetFrameHeight())) / 
		(((ImGui::GetWindowPos().y + ImGui::GetFrameHeight()) + (ImGui::GetWindowSize().y - ImGui::GetFrameHeight())) - (ImGui::GetWindowPos().y + ImGui::GetFrameHeight()))) - 0.5f) / 0.5f);

	if ((normalX >= -1 && normalX <= 1) && (normalY >= -1 && normalY <= 1))
	{
		LineSegment picking = cameraFrustum.UnProjectLineSegment(normalX, normalY);
		RayToMeshIntersection(picking);
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::RayToMeshIntersection(LineSegment ray)
{
	std::map<float, GameObject*> canSelect, distMap;
	float nHit, fHit;
	bool selected = false;

	// Raycast hits everything and see if it has AABB and can be selected. 
	for (std::vector<GameObject*>::iterator i = App->scene->root->children.begin(); i != App->scene->root->children.end(); ++i)
		if ((*i)->name != "Camera") if (ray.Intersects((*i)->globalAABB, nHit, fHit)) canSelect[nHit] = (*i);
	

	// Add all meshes with a triangle hit and store the distance from the ray to the triangle, then pick the closest one
	for (auto i = canSelect.begin(); i != canSelect.end(); ++i) // If it dosen't detect any object with AABB, it will skip this.
	{
		ComponentMesh* mesh = (*i).second->GetComponent<ComponentMesh>();
		if (mesh)
		{
			ray.Transform((*i).second->transform->transformMatrix.Inverted());

			if (mesh->numVertices >= 9)
			{
				for (int index = 0; index < mesh->numIndices; index += 3)
				{
					Triangle trian(mesh->vertices[mesh->indices[index]], 
						mesh->vertices[mesh->indices[index + 1]], 
						mesh->vertices[mesh->indices[index + 2]]);

					float dist = 0;
					if (ray.Intersects(trian, &dist, nullptr))
						distMap[dist] = (*i).second;
				}
			}
		}
	}
	canSelect.clear();

	// Select object in editor.
	if (distMap.begin() != distMap.end())
	{
		App->editor->gameobjectSelected = (*distMap.begin()).second;
		LOG("GameObject selected name: %s", (*distMap.begin()).second->name.c_str());
		selected = true;
	}
	distMap.clear();

	//If nothing is selected, set selected GO to null
	if (!selected)
		App->editor->gameobjectSelected = nullptr;
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& point)
{		
	reference = point;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}



// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	if (projectionIsDirty)
		RecalculateProjection();
	cameraFrustum.pos = position;
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void ModuleCamera3D::RecalculateProjection()
{
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

void ModuleCamera3D::OnGui()
{
	if (ImGui::CollapsingHeader("Editor Camera"))
	{
		ImGui::DragFloat("Near Distance", &cameraFrustum.nearPlaneDistance);
		ImGui::DragFloat("Far Distance", &cameraFrustum.farPlaneDistance);
		if (ImGui::DragFloat("Vertical FOV", &verticalFOV)) RecalculateProjection();

		if (cameraFrustum.nearPlaneDistance < 1) cameraFrustum.nearPlaneDistance = 1;
		else if (cameraFrustum.nearPlaneDistance >= cameraFrustum.farPlaneDistance)
			cameraFrustum.nearPlaneDistance = cameraFrustum.farPlaneDistance - 30;

		if (cameraFrustum.farPlaneDistance < 30) cameraFrustum.farPlaneDistance = 30;
		else if (cameraFrustum.farPlaneDistance <= cameraFrustum.nearPlaneDistance)
			cameraFrustum.farPlaneDistance = cameraFrustum.nearPlaneDistance + 30;

		if (verticalFOV <= 1) verticalFOV = 1;
		else if (verticalFOV >= 359) verticalFOV = 359;
	}
}

void ModuleCamera3D::OnSave(JSONWriter& writer) const
{
	writer.String("camera");	
	writer.StartObject();
	SAVE_JSON_FLOAT(verticalFOV)
	SAVE_JSON_FLOAT(nearPlaneDistance)
	SAVE_JSON_FLOAT(farPlaneDistance)
	SAVE_JSON_FLOAT(cameraSpeed)
	SAVE_JSON_FLOAT(cameraSensitivity)
	writer.EndObject();
}

void ModuleCamera3D::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("camera"))
	{
		const auto& config = reader["camera"];
		LOAD_JSON_FLOAT(verticalFOV);
		LOAD_JSON_FLOAT(nearPlaneDistance);
		LOAD_JSON_FLOAT(farPlaneDistance);
		LOAD_JSON_FLOAT(cameraSpeed);
		LOAD_JSON_FLOAT(cameraSensitivity);
	}
	RecalculateProjection();
}
