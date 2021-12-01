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
#include "pcg-c/include/pcg_basic.h"

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

bool ModuleImport::LoadGeometry(const char* path) 
{
	//Create path buffer and import to scene
	const aiScene* scene = nullptr;
	char* buffer = nullptr;
	uint bytesFile = App->fileSystem->Load(path, &buffer);

	if (buffer == nullptr) 
	{
		std::string normPathShort = App->fileSystem->SetNameFile(path);
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

			// -- Store header --//
			unsigned int NumIndices = 0;
			if (assimpMesh->HasFaces())
				NumIndices = assimpMesh->mNumFaces * 3;

			unsigned int NumNormals = 0;
			if (assimpMesh->HasNormals())
				NumNormals = assimpMesh->mNumVertices;

			unsigned int NumCoords = 0;
			if (assimpMesh->HasTextureCoords(0))
				NumCoords = assimpMesh->mNumVertices;

			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &assimpMesh->mNumVertices);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &NumIndices);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &NumNormals);
			StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &NumCoords);

			// -- Store name --//
			std::string name;
			char charName[1024];
			FindNodeName(scene, i, name);
			strncpy(charName, name.c_str(), sizeof(charName));

			StoreInBuffer(bytes, bytesPointer, sizeof(char) * 1024, &charName);

			// -- Store material --//
			if (scene->HasMaterials())
			{
				std::string texturePath;
				char charTexturePath[1024];

				aiMaterial* texture = scene->mMaterials[assimpMesh->mMaterialIndex];
				if (texture != nullptr)
				{
					aiString path;

					aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, assimpMesh->mMaterialIndex, &path);
					texturePath = path.C_Str();
					if (texturePath.size() > 0)
					{
						texturePath = App->fileSystem->SetNameFile(texturePath.c_str());
						std::string textureFile(texturePath);

						texturePath = "Library/Materials/" + App->fileSystem->SetNameFile(textureFile.c_str(), ".jay");

						if (!App->fileSystem->Exists(texturePath))
							App->textures->SaveTexture(textureFile.c_str(), texturePath.c_str());
					}
				}
				strncpy(charTexturePath, texturePath.c_str(), sizeof(charTexturePath));

				StoreInBuffer(bytes, bytesPointer, sizeof(char) * 1024, &charTexturePath);
			}

			// -- Store vertex --//
			StoreInBuffer(bytes, bytesPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mVertices[0]);

			// -- Store index --//
			if (assimpMesh->HasFaces()) 
			{
				std::vector<uint> indices = std::vector<uint>(assimpMesh->mNumFaces * 3);

				for (size_t f = 0; f < assimpMesh->mNumFaces; f++)
				{
					if (assimpMesh->mFaces[f].mNumIndices != 3)
						LOG("WARNING, geometry face with != 3 indices!");

					indices[(f * 3)] = assimpMesh->mFaces[f].mIndices[0];
					indices[(f * 3) + 1] = assimpMesh->mFaces[f].mIndices[1];
					indices[(f * 3) + 2] = assimpMesh->mFaces[f].mIndices[2];
				}
				StoreInBuffer(bytes, bytesPointer, sizeof(uint) * NumIndices, &indices[0]);
			}

			// -- Store Normals info --//
			if (assimpMesh->HasNormals()) 
				StoreInBuffer(bytes, bytesPointer, sizeof(float3) * assimpMesh->mNumVertices, &assimpMesh->mNormals[0]);

			// -- Store UV info --//
			if (assimpMesh->HasTextureCoords(0))
			{
				std::vector<float2> coords = std::vector<float2>(assimpMesh->mNumVertices);

				for (size_t c = 0; c < assimpMesh->mNumVertices; ++c)
					coords[c] = {	assimpMesh->mTextureCoords[0][c].x, 
									assimpMesh->mTextureCoords[0][c].y	};

				StoreInBuffer(bytes, bytesPointer, sizeof(float2) * NumCoords, &coords[0]);
			}

			// -- Save file --//
			std::string pathShort = "Library/Meshes/" + std::to_string(pcg32_random()) + App->fileSystem->SetNameFile(path, ".jgg");
			App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);

			// -- Load file --//
			LoadMeshFile(pathShort.c_str());
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

void ModuleImport::LoadMeshFile(const char* pathfile)
{
	char* buffer;
	uint bytes = App->fileSystem->Load(pathfile, &buffer);

	if (bytes > 0)
	{
		// -- Header --//
		unsigned numVertices, numIndices, numNormals, numCoords;
		memcpy(&numVertices, &buffer[0], sizeof(unsigned int));
		memcpy(&numIndices, &buffer[sizeof(unsigned int)], sizeof(unsigned int));
		memcpy(&numNormals, &buffer[sizeof(unsigned int) * 2], sizeof(unsigned int));
		memcpy(&numCoords, &buffer[sizeof(unsigned int) * 3], sizeof(unsigned int));

		// -- Byte pointer --//
		unsigned nameOffset = 4 * sizeof(unsigned);
		unsigned textureOffset = nameOffset + (sizeof(char) * 1024);
		unsigned verticesOffset = textureOffset + (sizeof(char) * 1024);
		unsigned indicesOffset = verticesOffset + (sizeof(float3) * numVertices);
		unsigned normalsOffset = indicesOffset + (sizeof(uint) * numIndices);
		unsigned coordsOffset = normalsOffset + (sizeof(float3) * numNormals);

		// -- Create mesh --//
		char charName[1024];
		memcpy(&charName[0], &buffer[nameOffset], sizeof(char) * 1024);
		std::string name(charName);

		GameObject* newGameObject = App->scene->CreateGameObject(name.c_str());
		ComponentMesh* newMesh = newGameObject->CreateComponent<ComponentMesh>();


		char charTexturePath[1024];
		memcpy(&charTexturePath[0], &buffer[textureOffset], sizeof(char) * 1024);
		std::string texturePath(charTexturePath);
		newMesh->texturePath = texturePath;
		
		if (texturePath.size() > 0)
		{
			if (!App->textures->Find(texturePath))
			{
				const TextureObject& textureObject = App->textures->Load(texturePath);
				ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
				materialComp->SetTexture(textureObject);
			}
			else
			{
				const TextureObject& textureObject = App->textures->Get(texturePath);
				ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
				materialComp->SetTexture(textureObject);
			}
		}


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

		newMesh->GenerateBuffers();
		newMesh->GenerateBounds();
		newMesh->ComputeNormals();
	}

	RELEASE(buffer);
}

void ModuleImport::SaveMeshFile(GameObject* gameObject, const char* path, std::string name)
{
	std::vector<char> bytes;
	unsigned bytesPointer = 0;

	ComponentMesh* mesh = gameObject->GetComponent<ComponentMesh>();
	ComponentMaterial* texture = gameObject->GetComponent<ComponentMaterial>();

	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numVertices);
	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numIndices);
	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numVertices);	//Normals
	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numVertices);	//Coords

	char charName[1024];
	strcpy(charName, gameObject->name.c_str());
	StoreInBuffer(bytes, bytesPointer, sizeof(char) * 1024, &charName);

	char charTexturePath[1024];
	strcpy(charName, texture->GetTextureName().c_str());
	StoreInBuffer(bytes, bytesPointer, sizeof(char) * 1024, &charTexturePath);

	StoreInBuffer(bytes, bytesPointer, sizeof(float3) * mesh->numVertices, &mesh->vertices[0]);
	StoreInBuffer(bytes, bytesPointer, sizeof(uint) * mesh->numIndices, &mesh->indices[0]);
	StoreInBuffer(bytes, bytesPointer, sizeof(float3) * mesh->numVertices, &mesh->normals[0]);
	StoreInBuffer(bytes, bytesPointer, sizeof(float3) * mesh->numVertices, &mesh->texCoords[0]);

	if (name.size() > 0)
	{
		std::string pathShort = path + App->fileSystem->SetNameFile(name.c_str(), ".fbx");
		App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);
	}
	else
	{
		std::string pathShort = path + App->fileSystem->SetNameFile(gameObject->name.c_str(), ".fbx");
		App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);
	}

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