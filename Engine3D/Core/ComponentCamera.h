#pragma once

#include "Component.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "Globals.h"
#include "ImGui/imgui.h"

class ComponentCamera : public Component 
{
public:
	ComponentCamera(GameObject* parent);
	ComponentCamera(GameObject* parent, bool CanCam);
	~ComponentCamera();
	
	void LookAt(const float3& point);
	void CalculateViewMatrix();
	void RecalculateProjection();
	void OnGui() override;
	//void OnSave(JSONWriter& writer);
	//void OnLoad(const JSONReader& reader);

	float3 right, up, front, position, reference;
	Frustum cameraFrustum;
	float4x4 viewMatrix;
	float aspectRatio = 1.f;
	float verticalFOV = 100.f;
	float nearPlaneDistance = 1.f;
	float farPlaneDistance = 5000.f;
	bool projectionIsDirty = false;

private:

	float lastDeltaX = 0.f, lastDeltaY = 0.f;
};
