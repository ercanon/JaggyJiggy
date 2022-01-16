#pragma once

#include "Component.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"
#include "glmath.h"


class ComponentTransform : public Component 
{
public:

	ComponentTransform(GameObject* parent);

	bool Update(float dt) override;
	void OnGui() override;

	void Move(float3 positionIncrease);

	void SetPosition(const float3& newPosition);
	void SetRotation(const float3& newRotation);
	void SetScale(const float3& newScale);
	void SetLocalTransform(const mat4x4& transform);

	inline float3 GetPosition() const { return position; };
	inline float3 GetRotation() const { return rotationEuler; };
	inline float3 GetFront() const { return transformMatrix.RotatePart().Col(2).Normalized(); };
	inline float3 GetUp() const { return transformMatrix.RotatePart().Col(1).Normalized();};
	inline float3 GetScale() const { return scale; };

	const mat4x4 GetGlobalGLTransform() const; // float4x4 -> mat4x4

	void NewAttachment();
	void OnParentMoved();

	void RecomputeGlobalMatrix();
	
	float4x4 transformMatrix;
	float4x4 transformMatrixLocal;

private:
	
	bool isDirty = false;

	float3 position;
	Quat rotation;
	float3 rotationEuler;
	float3 scale;

};