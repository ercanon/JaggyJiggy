#pragma once

// -- Tools
#include "Module.h"
#include "Globals.h"
#include "ModuleImport.h"
#include "ComponentCamera.h"

#include "GameObject.h"
class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	GameObject* CreateGameObject(GameObject* parent = nullptr);	
	GameObject* CreateGameObject(const std::string name, GameObject* parent = nullptr);	
	GameObject* SearchGameObject(const std::string name);
	
public:
	
	GameObject* root;
};
