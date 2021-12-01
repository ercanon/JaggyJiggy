#pragma once
#include "Globals.h"

#include <string>

class ModuleViewportFrameBuffer
{
public:
	ModuleViewportFrameBuffer();
	~ModuleViewportFrameBuffer();

	void Start();
	void PreUpdate();
	void PostUpdate();
	void CleanUp();

public:

	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint texture = 0;
	bool show_viewport_window = true;

};