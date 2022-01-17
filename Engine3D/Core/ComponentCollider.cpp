#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Application.h"
#include "ModulePhysics3D.h"

#include "ImGui/imgui.h"


ComponentCollider::ComponentCollider(GameObject* parent, Shape shape) : Component(parent) 
{
	switch (shape)
	{
	case Shape::CUBE:
		shapeCollider = "CUBE";

		ComponentMesh* mesh = parent->GetComponent<ComponentMesh>();
		if (mesh != nullptr)
		{
			AABB bbox = parent->globalAABB;
			float3 corners[8];
			bbox.GetCornerPoints(corners);
			float width = corners[0].Distance(corners[4]);
			float height = corners[1].Distance(corners[3]);
			float depth = corners[3].Distance(corners[2]);
			const btVector3 vec(width / 2, height / 2, depth / 2);
			btBoxShape* bs = new btBoxShape(btVector3(vec));
			App->physics->AddBody(bs, parent);
		}
		else
		{
			const btVector3 vec(1 / 2, 1 / 2, 1 / 2);
			new btBoxShape(btVector3(vec));
		}

		break;
	//case Shape::SPHERE:
	//	shapeCollider = "SPHERE";
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
	//	break;
	//case Shape::PLANE:
	//	shapeCollider = "PLANE";
	//	break;
	//case Shape::PYRAMID:
	//	shapeCollider = "PYRAMID";
	//	break;
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
		ImGui::DragFloat("Masa", &mass);
	}
}


