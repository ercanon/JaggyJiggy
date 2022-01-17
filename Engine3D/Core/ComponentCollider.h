#pragma once
#include "Component.h"
#include "Globals.h"
#include "ModulePhysics3D.h"
#include <string.h>
#include "PhysBody3D.h"

#include "btBulletDynamicsCommon.h"

class ComponentCollider : public Component {

public:

	enum class Shape
	{
		CUBE,
		SPHERE
	};

	ComponentCollider(GameObject* parent, Shape shape);
	~ComponentCollider();

	bool Update(float dt) override;
	void OnGui() override;

public:
	Shape shapeCol;
	std::string shapeCollider;
	PhysBody3D body;

private:
	float mass = 1;
	
	btDiscreteDynamicsWorld* world;
};