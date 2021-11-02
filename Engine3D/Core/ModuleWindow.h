#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;
	void OnLoad(const JSONReader& reader) override;
	void OnSave(JSONWriter& writer) const override;

	void OnGui() override;
	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	SDL_GLContext context;

	//ImGui control variables
	Uint32 flags;
	int width;
	int height;
	float brightness;
	bool resizable = true;
	bool fullscreen = false;
	bool borderless = true;
	bool fulldesktop = true;

	//Aspect Ratio
	float window_aspect_ratio;

};

#endif // __ModuleWindow_H__