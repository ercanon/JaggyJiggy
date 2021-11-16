#pragma once
#include "Module.h"

#include <string>

class ComponentMesh;
struct aiScene;

class ModuleImport : public Module
{
public:
	
	ModuleImport(Application* app, bool start_enabled = true);

	bool Init() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	bool LoadGeometry(const char* path);
	void StoreInBuffer(std::vector<char>& fileBuffer, uint& pointer, unsigned bytes, void* data);

	void FindNodeName(const aiScene* scene, const size_t i, std::string& name);


};