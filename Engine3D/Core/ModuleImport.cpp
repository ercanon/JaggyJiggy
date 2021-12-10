#include "Globals.h"

#include "Application.h"
#include "ModuleImport.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "GameObject.h"

#include <vector>
#include <queue>
#include "SDL/include/SDL_opengl.h"
#include "Math/float2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"

#include "rapidjson-1.1.0/include/rapidjson/document.h"
using namespace rapidjson;


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
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						indices[(f * 3)] = assimpMesh->mFaces[f].mIndices[0];
						indices[(f * 3) + 1] = assimpMesh->mFaces[f].mIndices[1];
						indices[(f * 3) + 2] = assimpMesh->mFaces[f].mIndices[2];
					}
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
			std::string pathShort = "Library/Meshes/" + App->fileSystem->SetNameFile(name.c_str(), ".jgg");
			App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);

			// -- Load file --//
			LoadMeshFile(pathShort.c_str(), scene, i);
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

void ModuleImport::LoadMeshFile(const char* pathfile, const aiScene* scene, const size_t i)
{
	char* buffer;
	uint bytes = App->fileSystem->Load(pathfile, &buffer);

	if (bytes > 0)
	{
		// -- Header --//
		unsigned int numVertices, numIndices, numNormals, numCoords;
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
		newMesh->GenerateBounds(true);
		newMesh->ComputeNormals();

		// -- Correction Transfrom --//
		aiMatrix4x4 matrix;
		TransformCorrection(scene->mMeshes[i], scene->mRootNode, matrix, scene);

		aiVector3D position, scale;
		aiQuaternion rotation;

		matrix.Decompose(scale, rotation, position);

		newGameObject->transform->SetPosition(float3(position.x, position.y, position.z));
		newGameObject->transform->SetRotation(Quat(rotation.x, rotation.y, rotation.z, rotation.w).ToEulerXYZ());
		newGameObject->transform->SetScale(float3(scale.x, scale.y, scale.z));
	}

	RELEASE(buffer);
}

/*
void ModuleImport::SaveMeshFile(GameObject* gameObject, std::string name)
{
	std::vector<char> bytes;
	unsigned bytesPointer = 0;

	ComponentMesh* mesh = gameObject->GetComponent<ComponentMesh>();

	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numVertices);
	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numIndices);
	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numVertices);	//Normals
	StoreInBuffer(bytes, bytesPointer, sizeof(unsigned int), &mesh->numVertices);	//Coords

	char charName[1024];
	strcpy(charName, gameObject->name.c_str());
	StoreInBuffer(bytes, bytesPointer, sizeof(char) * 1024, &charName);

	char charTexturePath[1024];
	strcpy(charName, gameObject->GetComponent<ComponentMaterial>()->GetTextureName().c_str());
	StoreInBuffer(bytes, bytesPointer, sizeof(char) * 1024, &charTexturePath);

	StoreInBuffer(bytes, bytesPointer, sizeof(float3) * mesh->numVertices, &mesh->vertices[0]);
	StoreInBuffer(bytes, bytesPointer, sizeof(uint) * mesh->numIndices, &mesh->indices[0]);
	StoreInBuffer(bytes, bytesPointer, sizeof(float3) * mesh->numVertices, &mesh->normals[0]);
	StoreInBuffer(bytes, bytesPointer, sizeof(float3) * mesh->numVertices, &mesh->texCoords[0]);
	

	if (name.size() > 0)
	{
		std::string pathShort = "Library/Meshes/" + App->fileSystem->SetNameFile(name.c_str(), ".jgg");
		App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);
	}
	else
	{
		std::string pathShort = "Library/Meshes/" + App->fileSystem->SetNameFile(gameObject->name.c_str(), ".jgg");
		App->fileSystem->Save(pathShort.c_str(), &bytes[0], bytesPointer);
	}
}
*/

void ModuleImport::SaveScene(const char* path)
{
	Document sceneFile;
	sceneFile.SetObject();
	StringBuffer bufferScene;
	Writer<StringBuffer> writer(bufferScene);
	Document::AllocatorType& allocator = sceneFile.GetAllocator();

	std::queue<GameObject*> T;
	for (GameObject* child : App->scene->root->children)
	{
		std::queue<GameObject*> C;
		C.push(child);
		while (!C.empty())
		{
			GameObject* go = C.front();
			T.push(go);
			C.pop();
			for (GameObject* child : go->children)
			{
				C.push(child);
			}
		}
	}
	/*
	// -- Textures --//
	Value textures(kArrayType);
	for (auto& t : App->textures->textures)
	{	
		textures.PushBack(StringRef(t.first.c_str()), allocator);
	}
	sceneFile.AddMember("Textures", textures, allocator);
	*/

	// -- GameObjects --//
	Value objectList(kArrayType);
	while (!T.empty())
	{
		GameObject* object = T.front();
		T.pop();
		Value currentObject(kObjectType);

		currentObject.AddMember("Name",
			StringRef(object->name.c_str()),
			allocator);
		currentObject.AddMember("Parent", 
			StringRef(object->parent->name.c_str()), 
			allocator);
		if (object->GetComponent<ComponentMaterial>())
		{
			Value texture;
			std::string stringTexture = object->GetComponent<ComponentMaterial>()->GetTextureName();
			texture.SetString(stringTexture.c_str(), stringTexture.length(), allocator);
			currentObject.AddMember("Material", texture, allocator);
		}
		if (ComponentMesh* compMesh = object->GetComponent<ComponentMesh>())
		{
			Value mesh(kObjectType);
			Value vertices(kArrayType);
			Value indices(kArrayType);
			Value normals(kArrayType);
			Value texCoords(kArrayType);

			for (size_t v = 0; v < compMesh->numVertices; v++)
			{
				Value vec(kArrayType);
				if (!compMesh->vertices.empty())
				{
					vec.PushBack(compMesh->vertices[v].x, allocator);
					vec.PushBack(compMesh->vertices[v].y, allocator);
					vec.PushBack(compMesh->vertices[v].z, allocator);
				}

				Value norm(kArrayType);
				if (!compMesh->normals.empty())
				{
					norm.PushBack(compMesh->normals[v].x, allocator);
					norm.PushBack(compMesh->normals[v].y, allocator);
					norm.PushBack(compMesh->normals[v].z, allocator);
				}

				Value coord(kArrayType);
				if (!compMesh->texCoords.empty())
				{
					coord.PushBack(compMesh->texCoords[v].x, allocator);
					coord.PushBack(compMesh->texCoords[v].y, allocator);
				}

				vertices.PushBack(vec, allocator);
				normals.PushBack(norm, allocator);
				texCoords.PushBack(coord, allocator);
			}
			for (size_t i = 0; i < compMesh->numIndices; i++)
				if (!compMesh->indices.empty())
					indices.PushBack(compMesh->indices[i], allocator);

			mesh.AddMember("NumVertices", compMesh->numVertices, allocator);
			mesh.AddMember("NumIndices", compMesh->numIndices, allocator);

			if (!compMesh->vertices.empty())
				mesh.AddMember("Vertices", vertices, allocator);
			if (!compMesh->indices.empty())
				mesh.AddMember("Indices", indices, allocator);
			if (!compMesh->normals.empty())
				mesh.AddMember("Normals", normals, allocator);
			if (!compMesh->texCoords.empty())
				mesh.AddMember("TexCoords", texCoords, allocator);

			currentObject.AddMember("Mesh", mesh, allocator);
		}
		if (object->transform->active)
		{
			ComponentTransform* transformObject = object->transform;
			Value transform(kObjectType);
			Value position(kArrayType);
			Value rotation(kArrayType);
			Value scale(kArrayType);

			position.PushBack(transformObject->GetPosition().x, allocator);
			position.PushBack(transformObject->GetPosition().y, allocator);
			position.PushBack(transformObject->GetPosition().z, allocator);

			rotation.PushBack(transformObject->GetRotation().x, allocator);
			rotation.PushBack(transformObject->GetRotation().y, allocator);
			rotation.PushBack(transformObject->GetRotation().z, allocator);

			scale.PushBack(transformObject->GetScale().x, allocator);
			scale.PushBack(transformObject->GetScale().y, allocator);
			scale.PushBack(transformObject->GetScale().z, allocator);

			transform.AddMember("Position", position, allocator);
			transform.AddMember("Rotation", rotation, allocator);
			transform.AddMember("Scale", scale, allocator);
			currentObject.AddMember("Transform", transform, allocator);
		}
		if (ComponentCamera* compCamera = object->GetComponent<ComponentCamera>())
		{
			Value camera(kObjectType);
			Value right(kArrayType);
			Value up(kArrayType);
			Value front(kArrayType);
			Value position(kArrayType);
			Value reference(kArrayType);

			right.PushBack(compCamera->right.x, allocator);
			right.PushBack(compCamera->right.y, allocator);
			right.PushBack(compCamera->right.z, allocator);

			up.PushBack(compCamera->up.x, allocator);
			up.PushBack(compCamera->up.y, allocator);
			up.PushBack(compCamera->up.z, allocator);

			front.PushBack(compCamera->front.x, allocator);
			front.PushBack(compCamera->front.y, allocator);
			front.PushBack(compCamera->front.z, allocator);

			position.PushBack(compCamera->position.x, allocator);
			position.PushBack(compCamera->position.y, allocator);
			position.PushBack(compCamera->position.z, allocator);

			reference.PushBack(compCamera->reference.x, allocator);
			reference.PushBack(compCamera->reference.y, allocator);
			reference.PushBack(compCamera->reference.z, allocator);

			camera.AddMember("Right", right, allocator);
			camera.AddMember("Up", up, allocator);
			camera.AddMember("Front", front, allocator);
			camera.AddMember("Position", position, allocator);
			camera.AddMember("Reference", reference, allocator);
			currentObject.AddMember("Camera", camera, allocator);
		}

		objectList.PushBack(currentObject, allocator);
	}
	sceneFile.AddMember("GameObjects", objectList, allocator);

	sceneFile.Accept(writer);
	const char* buffer = bufferScene.GetString();
	std::string pathScene = "Library/Scenes/" + App->fileSystem->SetNameFile("scene", ".jiy");
	App->fileSystem->Save(pathScene.c_str(), &buffer[0], strlen(buffer));
	LOG("Scene saved at %s", pathScene.c_str());
}

void ModuleImport::LoadScene(const char* path)
{
	char* json;
	uint bytes = App->fileSystem->Load(path, &json);

	if (bytes > 0)
	{
		Document sceneFile;
		sceneFile.Parse(json);

		/*
		for (int t = 0; t < sceneFile["Textures"].Size(); t++)
		{
			std::string texturePath = sceneFile["Textures"][t].GetString();

			if (texturePath.size() > 0)
			{
				std::string name = "Assets/Textures/" + App->fileSystem->SetNameFile(texturePath.c_str(), ".png");
				if (!App->textures->SaveTexture(name, texturePath))
				{
					name = App->fileSystem->SetNameFile(texturePath.c_str(), ".jpg");
					App->textures->SaveTexture(name, texturePath);
				}
			}

			if (!App->textures->Find(texturePath))
				App->textures->Load(texturePath);
		}
		*/
		for (int go = 0; go < sceneFile["GameObjects"].Size(); go++)
		{
			std::string name = sceneFile["GameObjects"][go]["Name"].GetString();
			std::string parentName = sceneFile["GameObjects"][go]["Parent"].GetString();

			GameObject* newGameObject = App->scene->CreateGameObject(name.c_str(), App->scene->SearchGameObject(parentName));

			if (sceneFile["GameObjects"][go].HasMember("Material"))
			{
				ComponentMaterial* newMaterial = newGameObject->CreateComponent<ComponentMaterial>();
				std::string texturePath = sceneFile["GameObjects"][go]["Material"].GetString();
				
				TextureObject texture;
				if (App->textures->Find(texturePath))
					texture = App->textures->Get(texturePath);
				else texture = App->textures->Load(texturePath);
				
				newMaterial->SetTexture(texture);
			}
			if (sceneFile["GameObjects"][go].HasMember("Transform"))
			{
				newGameObject->transform->SetPosition(float3(
					sceneFile["GameObjects"][go]["Transform"]["Position"][0].GetFloat(),
					sceneFile["GameObjects"][go]["Transform"]["Position"][1].GetFloat(),
					sceneFile["GameObjects"][go]["Transform"]["Position"][2].GetFloat()));
				newGameObject->transform->SetRotation(float3(
					sceneFile["GameObjects"][go]["Transform"]["Rotation"][0].GetFloat(),
					sceneFile["GameObjects"][go]["Transform"]["Rotation"][1].GetFloat(),
					sceneFile["GameObjects"][go]["Transform"]["Rotation"][2].GetFloat()));
				newGameObject->transform->SetScale(float3(
					sceneFile["GameObjects"][go]["Transform"]["Scale"][0].GetFloat(),
					sceneFile["GameObjects"][go]["Transform"]["Scale"][1].GetFloat(),
					sceneFile["GameObjects"][go]["Transform"]["Scale"][2].GetFloat()));
			}
			if (sceneFile["GameObjects"][go].HasMember("Mesh"))
			{
				ComponentMesh* newMesh = newGameObject->CreateComponent<ComponentMesh>();

				newMesh->numVertices = sceneFile["GameObjects"][go]["Mesh"]["NumVertices"].GetInt();
				newMesh->numIndices = sceneFile["GameObjects"][go]["Mesh"]["NumIndices"].GetInt();

				if (sceneFile["GameObjects"][go]["Mesh"].HasMember("Vertices"))
					newMesh->vertices.resize(newMesh->numVertices);
				if (sceneFile["GameObjects"][go]["Mesh"].HasMember("Indices"))
					newMesh->indices.resize(newMesh->numIndices);
				if (sceneFile["GameObjects"][go]["Mesh"].HasMember("Normals"))
					newMesh->normals.resize(newMesh->numVertices);
				if (sceneFile["GameObjects"][go]["Mesh"].HasMember("TexCoords"))
					newMesh->texCoords.resize(newMesh->numVertices);
				for (size_t v = 0; v < newMesh->numVertices; v++)
				{
					if (sceneFile["GameObjects"][go]["Mesh"].HasMember("Vertices"))
					{
						newMesh->vertices[v].x = sceneFile["GameObjects"][go]["Mesh"]["Vertices"][v][0].GetFloat();
						newMesh->vertices[v].y = sceneFile["GameObjects"][go]["Mesh"]["Vertices"][v][1].GetFloat();
						newMesh->vertices[v].z = sceneFile["GameObjects"][go]["Mesh"]["Vertices"][v][2].GetFloat();
					}

					if (sceneFile["GameObjects"][go]["Mesh"].HasMember("Normals"))
					{
						newMesh->normals[v].x = sceneFile["GameObjects"][go]["Mesh"]["Normals"][v][0].GetFloat();
						newMesh->normals[v].y = sceneFile["GameObjects"][go]["Mesh"]["Normals"][v][1].GetFloat();
						newMesh->normals[v].z = sceneFile["GameObjects"][go]["Mesh"]["Normals"][v][2].GetFloat();
					}

					if (sceneFile["GameObjects"][go]["Mesh"].HasMember("TexCoords"))
					{
						newMesh->texCoords[v].x = sceneFile["GameObjects"][go]["Mesh"]["TexCoords"][v][0].GetFloat();
						newMesh->texCoords[v].y = sceneFile["GameObjects"][go]["Mesh"]["TexCoords"][v][1].GetFloat();
					}
				}
				for (size_t i = 0; i < newMesh->numIndices; i++)
					if (sceneFile["GameObjects"][go]["Mesh"].HasMember("Indices"))
						newMesh->indices[i] = sceneFile["GameObjects"][go]["Mesh"]["Indices"][i].GetInt();

				newMesh->GenerateBuffers();
				newMesh->GenerateBounds(true);
				newMesh->ComputeNormals();
			}
			if (sceneFile["GameObjects"][go].HasMember("Camera"))
			{
				App->editor->newCam = new ComponentCamera(newGameObject, true);

				App->editor->newCam->right.x = sceneFile["GameObjects"][go]["Camera"]["Right"][0].GetFloat();
				App->editor->newCam->right.y = sceneFile["GameObjects"][go]["Camera"]["Right"][1].GetFloat();
				App->editor->newCam->right.z = sceneFile["GameObjects"][go]["Camera"]["Right"][2].GetFloat();

				App->editor->newCam->up.x = sceneFile["GameObjects"][go]["Camera"]["Up"][0].GetFloat();
				App->editor->newCam->up.y = sceneFile["GameObjects"][go]["Camera"]["Up"][1].GetFloat();
				App->editor->newCam->up.z = sceneFile["GameObjects"][go]["Camera"]["Up"][2].GetFloat();

				App->editor->newCam->front.x = sceneFile["GameObjects"][go]["Camera"]["Front"][0].GetFloat();
				App->editor->newCam->front.y = sceneFile["GameObjects"][go]["Camera"]["Front"][1].GetFloat();
				App->editor->newCam->front.z = sceneFile["GameObjects"][go]["Camera"]["Front"][2].GetFloat();

				App->editor->newCam->position.x = sceneFile["GameObjects"][go]["Camera"]["Position"][0].GetFloat();
				App->editor->newCam->position.y = sceneFile["GameObjects"][go]["Camera"]["Position"][1].GetFloat();
				App->editor->newCam->position.z = sceneFile["GameObjects"][go]["Camera"]["Position"][2].GetFloat();

				App->editor->newCam->reference.x = sceneFile["GameObjects"][go]["Camera"]["Reference"][0].GetFloat();
				App->editor->newCam->reference.y = sceneFile["GameObjects"][go]["Camera"]["Reference"][1].GetFloat();
				App->editor->newCam->reference.z = sceneFile["GameObjects"][go]["Camera"]["Reference"][2].GetFloat();
			}
		}
	}
	RELEASE(json);
}

void ModuleImport::TransformCorrection(const aiMesh* mesh, aiNode* node, aiMatrix4x4& matrix, const aiScene* scene)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		if (node->mChildren[i]->mNumMeshes > 0)
		{
			for (int j = 0; j < node->mChildren[i]->mNumMeshes; j++)
			{
				if (scene->mMeshes[node->mChildren[i]->mMeshes[j]] == mesh)
				{
					matrix = node->mChildren[i]->mTransformation;
				}
			}
		}
		if (node->mChildren[i]->mNumChildren > 0)
		{
			TransformCorrection(mesh, node->mChildren[i], matrix, scene);
		}
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