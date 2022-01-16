#pragma once
#include "Component.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/TransformOps.h"
#include "glmath.h"

class btRigidBody;
class btQuaternion;
class Module;

class ComponentCollider : public Component {

public:

	ComponentCollider(GameObject* parent);
	ComponentCollider(btRigidBody* body);
	~ComponentCollider();

	// Methods
	bool Update(float dt) override;
	void OnGui();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void RotateBody(btQuaternion rotationQuaternion);
	void CreateBody(btRigidBody* body);

	void OnUpdateTransform();

public:
	std::vector<Module*> collision_listeners;
	btRigidBody* body = nullptr;
	float mass = 0.0f;

	bool followObject = false;
	bool dynamicObject = false;
	float3 localPosition = float3::zero;
};