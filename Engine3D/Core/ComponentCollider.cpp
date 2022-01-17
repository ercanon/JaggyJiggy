#include "ComponentCollider.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Application.h"

#include "ImGui/imgui.h"


ComponentCollider::ComponentCollider(GameObject* parent, Shape shape) : Component(parent)
{
	shapeCol = shape;

	if (shape == Shape::CUBE)
	{
		if (parent->GetComponent<ComponentMesh>() != nullptr)
		{
			AABB bbox = parent->globalAABB;
			float3 corners[8];
			bbox.GetCornerPoints(corners);
			float width = corners[0].Distance(corners[4]) / 2;
			float heigth = corners[1].Distance(corners[3]) / 2;
			float depth = corners[3].Distance(corners[2]) / 2;

			CubeP* c = new CubeP(width, depth, heigth);
			float3 pos = parent->GetComponent<ComponentTransform>()->GetPosition();
			float3 rot = parent->GetComponent<ComponentTransform>()->GetRotation();
			c->SetPos(pos.x, pos.y, pos.z);
			body = c->body;
			body.SetBody(c, mass);
		}
		else
		{
			CubeP* c = new CubeP(1, 1, 1);
			float3 pos = parent->GetComponent<ComponentTransform>()->GetPosition();
			c->SetPos(pos.x, pos.y, pos.z);
			body = c->body;
			body.SetBody(c, mass);
		}
	}
	else if (shape == Shape::SPHERE)
	{
		if (parent->GetComponent<ComponentMesh>() != nullptr)
		{
			AABB bbox = parent->globalAABB;
			float3 corners[8];
			bbox.GetCornerPoints(corners);
			float width = corners[0].Distance(corners[4]);

			SphereP* s = new SphereP(width);
			float3 pos = parent->GetComponent<ComponentTransform>()->GetPosition();
			s->SetPos(pos.x, pos.y, pos.z);
			body = s->body;
			body.SetBody(s, mass);
		}
		else
		{
			SphereP* s = new SphereP(1);
			float3 pos = parent->GetComponent<ComponentTransform>()->GetPosition();
			s->SetPos(pos.x, pos.y, pos.z);
			body = s->body;
			body.SetBody(s, mass);
		}
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
		if(ImGui::Button("Collider Box"))
		{
			body.RemoveBody();
			body = nullptr;

			shapeCol = Shape::CUBE;
			CubeP* c = new CubeP(1, 1, 1);
			float3 pos = owner->parent->GetComponent<ComponentTransform>()->GetPosition();
			c->SetPos(pos.x, pos.y, pos.z);
			body = c->body;
			body.SetBody(c, mass);
		}
		if(ImGui::Button("Collider Sphere"))
		{
			body.RemoveBody();
			body = nullptr;

			shapeCol = Shape::SPHERE;

			SphereP* s = new SphereP(1);
			float3 pos = owner->parent->GetComponent<ComponentTransform>()->GetPosition();
			s->SetPos(pos.x, pos.y, pos.z);
			body = s->body;
			body.SetBody(s, mass);
		}
	}
}


