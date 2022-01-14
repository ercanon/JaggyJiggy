#pragma once
#include "Component.h"
#include "Globals.h"
#include <string.h>

class ComponentRB : public Component {

public:

	ComponentRB(GameObject* parent);
	~ComponentRB();

	bool Update(float dt) override;
	void OnGui() override;
};