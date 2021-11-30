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

bool ModuleImport::LoadGeometry(const char* path) 
{
	//Create path buffer and import to scene
	const aiScene* scene = nullptr;
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

			aiMesh* assimpMesh = scene->mMeshes[i];

			std::string name;
			FindNodeName(scene, i, name);
			unsigned int sizeName = name.size();

			unsigned int NumIndices = 0;
			if (assimpMesh->HasFaces())
				NumIndices = assimpMesh->mNumFaces * 3;

			unsigned int NumNormals = 0;
			if (assimpMesh->HasNormals())
				NumNormals = assimpMesh->mNumVertices;

			unsigned int NumCoords = 0;
			if (assimpMesh->HasTextureCoords(0))
				NumCoords = assimpMesh->mNumVertices;

			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &sizeName);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &assimpMesh->mNumVertices);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &NumIndices);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &NumNormals);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &NumCoords);

			// -- Store name --//
			const char* charName = name.c_str();
			StoreInBuffer(bytes, bytesPointer, sizeof(char*) * sizeName, &charName);

			// -- Store material --//
			if (scene->HasMaterials()) 
			{
				aiMaterial* texture = scene->mMaterials[assimpMesh->mMaterialIndex];

				if (texture != nullptr)
				{
					aiString texturePath;

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
			StoreInBuffer(bytes, bytesPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mVertices[0]);

			// -- Store index --//
			if (assimpMesh->HasFaces()) 
			{
				std::vector<uint> indices = std::vector<uint>(assimpMesh->mNumFaces * 3);

				for (size_t i = 0; i < assimpMesh->mNumFaces; i++)
				{
					if (assimpMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
						assert(assimpMesh->mFaces[i].mNumIndices == 3);
					}

					indices[(i * 3)] = assimpMesh->mFaces[i].mIndices[0];
					indices[(i * 3) + 1] = assimpMesh->mFaces[i].mIndices[1];
					indices[(i * 3) + 2] = assimpMesh->mFaces[i].mIndices[2];
				}
				StoreInBuffer(bytes, bytesPointer, sizeof(uint) * NumIndices, &indices[0]);
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

			// -- Save file --//
			std::string pathShort = "Library/Meshes/" + App->fileSystem->SetNormalName(path, true) + ".fuk";
			App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);

			// -- Load file --//
			ComponentMesh* mesh = LoadMeshFile(pathShort.c_str());
			if (mesh != nullptr)
			{
				mesh->GenerateBuffers();
				mesh->GenerateBounds();
				mesh->ComputeNormals();
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

ComponentMesh* ModuleImport::LoadMeshFile(const char* pathfile)
{
	char* buffer;
	uint bytes = App->fileSystem->Load(pathfile, &buffer);

	if (bytes > 0)
	{
		// -- Header --//
		unsigned sizeName, numVertices, numIndices, numNormals, numCoords;
		memcpy(&sizeName, &buffer[0], sizeof(unsigned int));
		memcpy(&numVertices, &buffer[sizeof(unsigned int)], sizeof(unsigned int));
		memcpy(&numIndices, &buffer[sizeof(unsigned int) * 2], sizeof(unsigned int));
		memcpy(&numNormals, &buffer[sizeof(unsigned int) * 3], sizeof(unsigned int));
		memcpy(&numCoords, &buffer[sizeof(unsigned int) * 4], sizeof(unsigned int));

		// -- Byte pointer --//
		unsigned nameOffset = 5 * sizeof(unsigned);
		unsigned verticesOffset = nameOffset + (sizeof(char*) * sizeName);
		unsigned indicesOffset = verticesOffset + (sizeof(float3) * numVertices);
		unsigned normalsOffset = indicesOffset + (sizeof(uint) * numIndices);
		unsigned coordsOffset = normalsOffset + (sizeof(float3) * numNormals);
		unsigned materialsOffset = coordsOffset + (sizeof(float2) * numCoords);

		// -- Create mesh --//
		const char* name;
		memcpy(&name, &buffer[nameOffset], sizeof(char*) * sizeName);
		GameObject* newGameObject = App->scene->CreateGameObject(name);
		ComponentMesh* newMesh = newGameObject->CreateComponent<ComponentMesh>();

		newMesh->numVertices = numVertices;
		newMesh->numIndices = numIndices;

		newMesh->vertices.resize(numVertices);
		memcpy(&newMesh->vertices[0], &buffer[verticesOffset], sizeof(float3) * numVertices);

		if (numIndices > 0)
		{
			newMesh->indices.resize(numIndices);
			memcpy(&newMesh->indices[0], &buffer[indicesOffset], sizeof(uint) * numIndices);
		}

		if (numNormals > 0)
		{
			newMesh->normals.resize(numNormals);
			memcpy(&newMesh->normals[0], &buffer[normalsOffset], sizeof(float3) * numNormals);
		}

		if (numCoords > 0)
		{
			newMesh->texCoords.resize(numCoords);
			memcpy(&newMesh->texCoords[0], &buffer[coordsOffset], sizeof(float2) * numCoords);
		}

		RELEASE(buffer);
		return newMesh;
	}
	RELEASE(buffer);
	return nullptr;
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