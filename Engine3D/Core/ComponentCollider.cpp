#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "ImGui/imgui.h"


ComponentCollider::ComponentCollider(GameObject* parent, Shape shape) : Component(parent) 
{
	switch (shape)
	{
	case Shape::CUBE:
		shapeCollider = "CUBE";
		break;
	case Shape::SPHERE:
		shapeCollider = "SPHERE";
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
	case Shape::CAPSULE:
		shapeCollider = "CAPSULE";
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
		ImGui::Text("Shape %s",	shapeCollider.c_str());
		ImGui::InputFloat("Mass", &mass);
	}
}


