#include "ComponentRB.h"

#include "ImGui/imgui.h"


ComponentRB::ComponentRB(GameObject* parent) : Component(parent)
{

}

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


