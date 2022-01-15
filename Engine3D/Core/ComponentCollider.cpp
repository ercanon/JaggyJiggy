#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "ImGui/imgui.h"


#ifdef _DEBUG
#pragma comment (lib, "BulletDynamics_debug.lib")
#pragma comment (lib, "BulletCollision_debug.lib")
#pragma comment (lib, "LinearMath_debug.lib")
#else
#pragma comment (lib, "BulletDynamics.lib")
#pragma comment (lib, "BulletCollision.lib")
#pragma comment (lib, "LinearMath.lib")
#endif


ComponentCollider::ComponentCollider(GameObject* parent, Shape shape) : Component(parent) 
{
	switch (shape)
	{
	case Shape::CUBE:
		break;
	case Shape::SPHERE:
		//btCollisionShape* colShape = new btSphereShape(parent->GetComponent<ComponentTransform>()->GetScale().x);
		//
		//btTransform startTransform;
		//startTransform.setFromOpenGLMatrix(&sphere.transform);
		//
		//btVector3 localInertia(0, 0, 0);
		//colShape->calculateLocalInertia(mass, localInertia);
		//
		//btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		//
		//btRigidBody* body = new btRigidBody(rbInfo);
		//
		//body->setUserPointer(parent);
		//world->addRigidBody(body);
		break;
	case Shape::PLANE:
		break;
	case Shape::PYRAMID:
		break;
	}
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
		ImGui::DragFloat("Masa", &mass);
	}
}


