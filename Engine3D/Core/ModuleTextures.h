#pragma once

#include <map>
#include <string>
#include "Module.h"

struct TextureObject
{
	TextureObject() = default;

	TextureObject(std::string name, uint id, uint width, uint height) : name(name), id(id), width(width), height(height) {};
	std::string name;
	uint id = 0;
	uint width = 0, height = 0;
};


class ModuleTextures : public Module
{
public:

	ModuleTextures(Application* app, bool start_enabled = true);

	bool Start() override;
	bool CleanUp() override;

	const bool SaveTexture(const std::string& pathOrigin, const std::string& pathDestiny, bool useMipMaps = false);
	const TextureObject& Load(const std::string& pathOrigin, bool useMipMaps = false);
	void Save(const char* path);
	const TextureObject& Get(const std::string& path);

	bool Find(const std::string& path) const;

	uint32 whiteFallback = 0, blackFallback = 0, checkers = 0;

	std::map<const std::string, TextureObject> textures;
};