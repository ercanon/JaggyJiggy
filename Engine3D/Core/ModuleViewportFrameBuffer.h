#pragma once
#include "Module.h"
#include "Globals.h"

#include <string>

class ModuleViewportFrameBuffer : public Module
{
public:
	ModuleViewportFrameBuffer(Application* app, bool start_enabled = true);
	ModuleViewportFrameBuffer();
	~ModuleViewportFrameBuffer();

	bool Init() override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp();

public:

	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint texture = 0;
	bool show_viewport_window = true;

};