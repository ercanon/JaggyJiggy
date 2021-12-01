#include "ComponentCamera.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"

//ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent) {
//
//	cameraFrustum.type = FrustumType::PerspectiveFrustum;
//	cameraFrustum.nearPlaneDistance = 1.f;
//	cameraFrustum.farPlaneDistance = 5000.f;
//	cameraFrustum.up = float3(0.0f, 1.0f, 0.0f);
//	cameraFrustum.front = float3(0.0f, 0.0f, 1.0f); 
//	cameraFrustum.pos = float3(0.0f, 5.0f, -15.0f);
//	cameraFrustum.verticalFov = 60.0f * DEGTORAD;
//	cameraFrustum.horizontalFov = 2.0f * atanf(tanf(cameraFrustum.verticalFov / 2.0f) * 1.3f);
//	
//}	
//// NOW THIS!
//bool ComponentCamera::Update(float dt) 
//{
//	
//	return UPDATE_CONTINUE;
//}
//
//void LookAt(const float3& point) {
//
//}
//
//void CalculateViewMatrix() {
//
//}
//
//void RecalculateProjection() {
//
//}
//
//void OnGui() {
//
//}
//
//void OnSave(JSONWriter& writer) {
//
//}
//
//void OnLoad(const JSONReader& reader) {
//
//}