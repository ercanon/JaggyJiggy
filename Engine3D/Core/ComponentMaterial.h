#pragma once

#include "Component.h"
#include "Assimp/include/material.h"
#include "Devil/include/il.h"

struct TextureObject;

namespace MaterialImporter
{
	void Import(const aiMaterial* material, TextureObject* ourMaterial);
	uint64 Save(const TextureObject* ourMaterial, char** fileBuffer);
	void Load(const char* fileBuffer, TextureObject* ourMaterial);
};

class ComponentMaterial : public Component {

public:

	ComponentMaterial(GameObject* parent);

	void SetTexture(const TextureObject& texture);
	void OnGui() override;
	inline uint GetTextureId() const { return textureId; }

private:

	std::string textureName;
	uint textureId = 0, width = 0, height = 0;
};