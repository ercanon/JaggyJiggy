#pragma once
#include "Component.h"
#include "Globals.h"
#include <string.h>

class ComponentCollider : public Component {

public:

	enum class Shape
	{
		CUBE,
		SPHERE,
		PLANE,
		PYRAMID
	};

	ComponentCollider(GameObject* parent, Shape shape);
	~ComponentCollider();

	bool Update(float dt) override;
	void OnGui() override;
};