#pragma once
#include "Component.h"
#include "Globals.h"
#include <string.h>

#include "btBulletDynamicsCommon.h"

class ComponentCollider : public Component {

public:

	enum class Shape
	{
		CUBE,
		SPHERE,
		CAPSULE
	};

	ComponentCollider(GameObject* parent, Shape shape);
	~ComponentCollider();

	bool Update(float dt) override;
	void OnGui() override;

public:
	std::string shapeCollider;

private:
	float mass = 1;

	btDiscreteDynamicsWorld* world;
};