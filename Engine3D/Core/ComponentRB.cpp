#include "ComponentRB.h"

#include "ImGui/imgui.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"


ComponentRB::ComponentRB(GameObject* parent) : Component(parent) {}

ComponentRB::~ComponentRB()
{

}

bool ComponentRB::Update(float dt)
{

	return true;
}

void ComponentRB::OnGui()
{
	if (ImGui::CollapsingHeader("Rigid Body"))
	{

	}
}


