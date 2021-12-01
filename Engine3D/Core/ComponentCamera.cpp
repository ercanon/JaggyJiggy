#include "ComponentCamera.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent) {}	

ComponentCamera::ComponentCamera(GameObject* parent, bool CanCam) : Component(parent)
{
	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(20.0f, 10.0f, -30.0f);
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

	if (ImGui::CollapsingHeader("TestCam"))
	{
	}
}

/*void OnSave(JSONWriter& writer) { //Needed?

}

void OnLoad(const JSONReader& reader) { //Needed?

}*/
