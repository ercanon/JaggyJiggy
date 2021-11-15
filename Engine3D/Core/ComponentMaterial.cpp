#include "Application.h"
#include "ModuleTextures.h"
#include "ImGui/imgui.h"
#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent) {}

void ComponentMaterial::SetTexture(const TextureObject& texture)
{
	textureName = texture.name;
	textureId = texture.id;
	width = texture.width;
	height = texture.height;
}

void ComponentMaterial::OnGui()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		if (textureId != 0)
		{
			ImGui::Text("Name: %s", textureName.c_str());
			ImGui::Image((ImTextureID)textureId, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Text("Size: %d x %d", width, height);

			if (ImGui::TreeNode("Texture List"))
			{
				for (auto& t : App->textures->textures)
				{
					ImGui::PushID(t.second.id);
					if (ImGui::ImageButton((ImTextureID)t.second.id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0)))
					{
						SetTexture(t.second);
					}
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
		}
	}
}

uint64 MaterialImporter::Save(const TextureObject* ourMaterial, char** fileBuffer)
{
	ILuint size;
	ILubyte* data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			*fileBuffer = (char*)data;
		RELEASE_ARRAY(data);
	}
}

