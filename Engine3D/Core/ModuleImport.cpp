#include "Globals.h"

#include "Application.h"
#include "ModuleImport.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "GameObject.h"

#include <vector>
#include <queue>
#include "SDL/include/SDL_opengl.h"
#include "Math/float2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"


ModuleImport::ModuleImport(Application* app, bool start_enabled) : Module(app, start_enabled) {}


// Called before render is available
bool ModuleImport::Init()
{
	//LOG("Creating 3D Renderer context");
	bool ret = true;

	//Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

update_status ModuleImport::Update(float dt) {

	return UPDATE_CONTINUE;
}

/*
bool ModuleImport::LoadGeometry(const char* path) 
{
	//-- Own structure	
	GameObject* root = nullptr;
	std::string new_root_name(path);

	//-- Assimp stuff
	aiMesh* assimpMesh = nullptr;
	const aiScene* scene = nullptr;
	aiMaterial* texture = nullptr;
	aiString texturePath;

	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = App->fileSystem->Load(path, &buffer);

	if (buffer == nullptr) 
	{
		std::string normPathShort = "Assets/Models/" + App->fileSystem->SetNormalName(path);
		bytesFile = App->fileSystem->Load(normPathShort.c_str(), &buffer);
	}
	if (buffer != nullptr) 
	{
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	}
	else 
	{
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	}


	if (scene != nullptr && scene->HasMeshes()) {
		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{		
			std::vector<char> fileBuffer;
			uint bufferPointer = 0;

			bool nameFound = false;
			std::string name;
			FindNodeName(scene, i, name);

			GameObject* newGameObject = App->scene->CreateGameObject(name);
			ComponentMesh* mesh = newGameObject->CreateComponent<ComponentMesh>();
			assimpMesh = scene->mMeshes[i];
			
			if (scene->HasMaterials()) 
			{
				texture = scene->mMaterials[assimpMesh->mMaterialIndex];

				if (texture != nullptr) 
				{
					aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, assimpMesh->mMaterialIndex, &texturePath);
					std::string new_path(texturePath.C_Str());
					if (new_path.size() > 0) 
					{
						mesh->texturePath = "Assets/Textures/" + new_path;
						if (!App->textures->Find(mesh->texturePath))
						{
							const TextureObject& textureObject = App->textures->Load(mesh->texturePath);							
							ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
							materialComp->SetTexture(textureObject);							
						}
						else
						{
							const TextureObject& textureObject = App->textures->Get(mesh->texturePath);
							ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
							materialComp->SetTexture(textureObject);
						}
					}
				}
			}
	
			mesh->numVertices = assimpMesh->mNumVertices;
			mesh->vertices.resize(assimpMesh->mNumVertices);
			
			memcpy(&mesh->vertices[0], assimpMesh->mVertices, sizeof(float3) * assimpMesh->mNumVertices);
			LOG("New mesh with %d vertices", assimpMesh->mNumVertices);

			StoreInBuffer(fileBuffer, bufferPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mVertices);

			// -- Copying faces --//
			if (assimpMesh->HasFaces()) 
			{
				mesh->numIndices = assimpMesh->mNumFaces * 3;
				mesh->indices.resize(mesh->numIndices);

				for (size_t i = 0; i < assimpMesh->mNumFaces; i++)
				{
					if (assimpMesh->mFaces[i].mNumIndices != 3) 
					{
						LOG("WARNING, geometry face with != 3 indices!")
					}
					else 
					{
						memcpy(&mesh->indices[i * 3], assimpMesh->mFaces[i].mIndices, sizeof(uint) * assimpMesh->mFaces[i].mNumIndices);

						StoreInBuffer(fileBuffer, bufferPointer, sizeof(uint) * assimpMesh->mFaces[i].mNumIndices, &assimpMesh->mFaces[i].mIndices);
					}
				}
			}
			
			// -- Copying Normals info --//
			if (assimpMesh->HasNormals()) 
			{
				mesh->normals.resize(assimpMesh->mNumVertices);
				memcpy(&mesh->normals[0], assimpMesh->mNormals, sizeof(float3) * assimpMesh->mNumVertices);

				StoreInBuffer(fileBuffer, bufferPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mNormals);
			}
			
			// -- Copying UV info --//
			if (assimpMesh->HasTextureCoords(0))
			{
				mesh->texCoords.resize(assimpMesh->mNumVertices);
				for (size_t j = 0; j < assimpMesh->mNumVertices; ++j)
				{
					memcpy(&mesh->texCoords[j], &assimpMesh->mTextureCoords[0][j], sizeof(float2));

					StoreInBuffer(fileBuffer, bufferPointer, sizeof(float2), &assimpMesh->mTextureCoords[0][j]);
				}
			}
			
			mesh->GenerateBuffers();
			mesh->GenerateBounds();
			mesh->ComputeNormals();
		}
		aiReleaseImport(scene);		
		RELEASE_ARRAY(buffer);
	}
	else 
		LOG("Error loading scene %s", path);

	RELEASE_ARRAY(buffer);

	return true;
}
*/

bool ModuleImport::LoadGeometry(const char* path) {

	//-- Assimp stuff
	aiMesh* assimpMesh = nullptr;
	const aiScene* scene = nullptr;
	aiMaterial* texture = nullptr;
	aiString texturePath;

	//Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = App->fileSystem->Load(path, &buffer);

	if (buffer == nullptr) 
	{
		std::string normPathShort = "Assets/Models/" + App->fileSystem->SetNormalName(path);
		bytesFile = App->fileSystem->Load(normPathShort.c_str(), &buffer);
	}
	if (buffer != nullptr) 
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	else scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);


	if (scene != nullptr && scene->HasMeshes()) {
		//Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			std::vector<char> bytes;
			uint bytesPointer = 0;

			assimpMesh = scene->mMeshes[i];

			// -- Store index --//
			StoreInBuffer(bytes, bytesPointer, sizeof(float3), &assimpMesh->mNumVertices);
			if (assimpMesh->HasFaces())
			{
				uint NumIndices = assimpMesh->mNumFaces * 3;
				StoreInBuffer(bytes, bytesPointer, sizeof(uint), &NumIndices);
			}
			if (assimpMesh->HasNormals())
				StoreInBuffer(bytes, bytesPointer, sizeof(float3), &assimpMesh->mNumVertices);
			if (assimpMesh->HasTextureCoords(0))
				StoreInBuffer(bytes, bytesPointer, sizeof(float2), &assimpMesh->mNumVertices);

			// -- Material --//
			if (scene->HasMaterials()) 
			{
				texture = scene->mMaterials[assimpMesh->mMaterialIndex];

				if (texture != nullptr)
				{
					aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, assimpMesh->mMaterialIndex, &texturePath);
					std::string new_path(texturePath.C_Str());
					if (new_path.size() > 0) 
					{
						std::string texturePath = "Assets/Textures/" + new_path;
						if (!App->textures->Find(texturePath))
							const TextureObject& textureObject = App->textures->Load(texturePath);
						else const TextureObject& textureObject = App->textures->Get(texturePath);
					}
				}
			}

			// -- Store vertex --//
			StoreInBuffer(bytes, bytesPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mVertices);

			// -- Store index --//
			if (assimpMesh->HasFaces()) 
			{
				for (size_t i = 0; i < assimpMesh->mNumFaces; i++)
				{
					if (assimpMesh->mFaces[i].mNumIndices != 3)
						LOG("WARNING, geometry face with != 3 indices!")
					else StoreInBuffer(bytes, bytesPointer, sizeof(uint) * assimpMesh->mFaces[i].mNumIndices, &assimpMesh->mFaces[i].mIndices);
				}
			}

			// -- Store Normals info --//
			if (assimpMesh->HasNormals()) 
				StoreInBuffer(bytes, bytesPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mNormals);

			// -- Store UV info --//
			if (assimpMesh->HasTextureCoords(0))
			{
				for (size_t j = 0; j < assimpMesh->mNumVertices; ++j)
					StoreInBuffer(bytes, bytesPointer, sizeof(float2), &assimpMesh->mTextureCoords[0][j]);
			}
		}
		aiReleaseImport(scene);
		RELEASE_ARRAY(buffer);
	}
	else LOG("Error loading scene %s", path);

	RELEASE_ARRAY(buffer);

	return true;
}

void ModuleImport::StoreInBuffer(std::vector<char>& fileBuffer, uint& pointer, unsigned bytes, void* data)
{
	//Resize the buffer each time new data is going to be stored
	fileBuffer.resize(fileBuffer.size() + bytes);
	memcpy(&fileBuffer[pointer], data, bytes);
	pointer += bytes;
}

void ModuleImport::FindNodeName(const aiScene* scene, const size_t i, std::string& name)
{
	bool nameFound = false;
	std::queue<aiNode*> Q;
	Q.push(scene->mRootNode);
	while (!Q.empty() && !nameFound)
	{
		aiNode* node = Q.front();
		Q.pop();
		for (size_t j = 0; j < node->mNumMeshes; ++j)
		{
			if (node->mMeshes[j] == i)
			{
				nameFound = true;
				name = node->mName.C_Str();
			}
		}
		if (!nameFound)
		{
			for (size_t j = 0; j < node->mNumChildren; ++j)
			{
				Q.push(node->mChildren[j]);
			}
		}
	}
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	//-- Detach log stream
	aiDetachAllLogStreams();

	return true;
}