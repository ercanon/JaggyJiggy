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
		CubeP* c = new CubeP(1, 1, 1);
		float3 pos = parent->GetComponent<ComponentTransform>()->GetPosition();
		c->SetPos(pos.x, pos.y, pos.z);
		c->body.SetBody(c, mass);
	}
	else if (shape == Shape::SPHERE)
	{
		SphereP* s = new SphereP(1);
		float3 pos = parent->GetComponent<ComponentTransform>()->GetPosition();
		s->SetPos(pos.x, pos.y, pos.z);
		body = s->body;
		body.SetBody(s, mass);
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


