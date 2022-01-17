#pragma once
#include "Component.h"
#include "Globals.h"
#include "ModulePhysics3D.h"
#include <string.h>

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

	std::string shapeCollider;
	Shape shapeCol;
	CubeP* c;
	SphereP* s;

private:
	float mass = 1;
	
	btDiscreteDynamicsWorld* world;
};