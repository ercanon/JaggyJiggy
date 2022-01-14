#include "ComponentCollider.h"

#include "ImGui/imgui.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"


ComponentCollider::ComponentCollider(GameObject* parent, Shape shape) : Component(parent) 
{

}

ComponentCollider::~ComponentCollider()
{

}

bool ComponentCollider::Update(float dt)
{


	return true;
}

void ComponentCollider::OnGui()
{
	if (ImGui::CollapsingHeader("Collider"))
	{

	}
}


