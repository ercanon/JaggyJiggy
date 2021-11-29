#pragma once

#include <vector>
#include <string>
#include "Geometry/OBB.h"
#include "Geometry/AABB.h"

class Component;
class ComponentTransform;

class GameObject {

public:

	GameObject();
	GameObject(const std::string name);

	~GameObject();

	void Update(float dt);
	void OnGui();

	template<class T> T* CreateComponent()
	{
		T* newComponent = new T(this);
		return newComponent;
	}

	template<class T> T* GetComponent()
	{
		T* component = nullptr; 
		for (Component* c : components)
		{
			component = dynamic_cast<T*>(c);
			if (component)
				break;
		}
		return component;
	}

	void DeleteComponent(Component* component);
	void AddComponent(Component* component);
	void AttachChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void PropagateTransform();

	std::string name;
	GameObject* parent = nullptr;
	ComponentTransform* transform = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;
	
	bool active = true;
	bool isSelected = false;

private:
	
	OBB globalOBB;
	AABB globalAABB;

};

