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
		c = new CubeP(1, 1, 1);
		c->SetPos(parent->GetComponent<ComponentTransform>()->GetPosition().x, parent->GetComponent<ComponentTransform>()->GetPosition().y, parent->GetComponent<ComponentTransform>()->GetPosition().z);
		c->body.SetBody(c, mass);
	}
	else if (shape == Shape::SPHERE)
	{
		s = new SphereP(1);
		s->SetPos(parent->GetComponent<ComponentTransform>()->GetPosition().x, parent->GetComponent<ComponentTransform>()->GetPosition().y, parent->GetComponent<ComponentTransform>()->GetPosition().z);
		s->body.SetBody(s, mass);
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
			s->body.RemoveBody();
			s = nullptr;

			shapeCol = Shape::CUBE;
			c = new CubeP(1, 1, 1);
			c->SetPos(owner->parent->GetComponent<ComponentTransform>()->GetPosition().x, owner->parent->GetComponent<ComponentTransform>()->GetPosition().y, owner->parent->GetComponent<ComponentTransform>()->GetPosition().z);
			c->body.SetBody(c, mass);
		}
		if(ImGui::Button("Collider Sphere"))
		{
			c->body.RemoveBody();
			c = nullptr;

			shapeCol = Shape::SPHERE;
			s = new SphereP(1);
			s->SetPos(owner->parent->GetComponent<ComponentTransform>()->GetPosition().x, owner->parent->GetComponent<ComponentTransform>()->GetPosition().y, owner->parent->GetComponent<ComponentTransform>()->GetPosition().z);
			s->body.SetBody(s, mass);
		}
	}
}


