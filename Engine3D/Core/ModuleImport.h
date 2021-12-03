#pragma once
#include "Module.h"

#include <string>

class ComponentMesh;
class GameObject;
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

	void LoadMeshFile(const char* pathfile, bool scene = false, char* bufferScene = nullptr);
	void SaveMeshFile(GameObject* gameObject, const char* path, std::string name = nullptr);

	void SaveScene(const char* path);
	void LoadScene(const char* path);

	void FindNodeName(const aiScene* scene, const size_t i, std::string& name);
};