#pragma once
#include "Module.h"
#include "Assimp/include/matrix4x4.h"

#include <string>

class ComponentMesh;
class GameObject;
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMartix4x4;

class ModuleImport : public Module
{
public:
	
	ModuleImport(Application* app, bool start_enabled = true);

	bool Init() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	bool LoadGeometry(const char* path);

	void StoreInBuffer(std::vector<char>& fileBuffer, uint& pointer, unsigned bytes, void* data);

	void LoadMeshFile(const char* pathfile, const aiScene* scene, const size_t i);
	void SaveMeshFile(GameObject* gameObject, std::string name = nullptr);

	void SaveScene(const char* path);
	void LoadScene(const char* path);

	void TransformCorrection(const aiMesh* mesh, aiNode* node, aiMatrix4x4& matrix, const aiScene* scene);
	void FindNodeName(const aiScene* scene, const size_t i, std::string& name);
};