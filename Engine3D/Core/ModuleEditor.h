#pragma once
#include "Module.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "ModuleTextures.h"
#include "ComponentCamera.h"
#include <string>
#include <stack>

//Forward declaration
class GameObject;
class ComponentTransform;
class File;

class ModuleEditor : public Module
{

	struct Grid
	{
		uint VAO;
		uint length;

		~Grid();
	};

	Grid grid;

public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Init() override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();
	
	void CreateGridBuffer();
	void DrawGrid();

	//Docking Helper functions
	bool DockingRootItem(char* id, ImGuiWindowFlags winFlags);
	void BeginDock(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size = { .0f, .0f });

	//Core Update functions to show and manage windows
	void MenuBar();
	void UpdateWindowStatus();

	//Console Text Pushback
	void UpdateText(const char* consoleText);

	void About_Window();	//Can be done better

	//Assets managment
	void AssetsArray();
	void DrawID(uint id, const char* text, int numID);
	void CleanUpObject();

	void InspectorGameObject();

	//Window status control
	bool showDemoWindow;
	bool showAnotherWindow;
	bool showAboutWindow;
	bool showConfWindow;
	bool showInspectorWindow;
	bool showHierarchyWindow;
	bool showSceneWindow;
	bool showGameWindow;
	bool showTextures;
	bool showConsoleWindow;
	bool showAssetsListWindow;
	bool showAssetsWindow;

	std::stack<GameObject*> S;
	std::stack<uint> indents;

	ImGuiTextBuffer consoleText;

	ImVec4 currentColor;

	ImGuiWindowFlags sceneWindow = 0;

	GameObject* gameobjectSelected;
	GameObject* go;

	// Cam Game
	ComponentCamera* newCam;

	ImVec2 lastViewportSize;
	ImVec2 lastViewportSize2;

	// Assets managment
	File* assets;
	File* assetFile;
	File* assetselect;

	// Assets ID
	uint folderID, pngID, jpgID, tgaID, fbxID;

	// Assets Image
	TextureObject folderImage, pngImage, jpgImage, tgaImage, fbxImage;

	std::vector<std::string> assetsString;
};