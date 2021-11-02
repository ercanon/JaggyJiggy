#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ImGui/imgui.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;

	width = 0;
	height = 0;
	brightness = 1.0f;
	
	//Aspect Ratio of the Window
	window_aspect_ratio = (float)(SCREEN_WIDTH / (float)SCREEN_HEIGHT);

	context = nullptr;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		//Create Window with default size
		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		//Update window  with brigtness value
		SDL_SetWindowBrightness(App->window->window, App->window->brightness);

		
		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			context = SDL_GL_CreateContext(window);
		}
	}


	return ret;
}

bool ModuleWindow::Start()
{
	if (fullscreen) 
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	else
		SDL_SetWindowFullscreen(window, flags); 
	
	if (resizable)
		SDL_SetWindowResizable(window, SDL_TRUE);
	else 
		SDL_SetWindowResizable(window, SDL_FALSE); 

	if (borderless) 
		SDL_SetWindowBordered(window, SDL_FALSE);
	else 
		SDL_SetWindowBordered(window, SDL_TRUE); 
		
	if (fulldesktop) 
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else 
		SDL_SetWindowFullscreen(window, flags);
	
	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("window"))
	{
		const auto& config = reader["window"];
		LOAD_JSON_BOOL(fullscreen)
		LOAD_JSON_BOOL(fulldesktop)
		LOAD_JSON_BOOL(borderless)
		LOAD_JSON_BOOL(resizable)
	}
}

void ModuleWindow::OnSave(JSONWriter& writer) const
{
	writer.String("window");
	writer.StartObject();
	SAVE_JSON_BOOL(fullscreen)
	SAVE_JSON_BOOL(fulldesktop)
	SAVE_JSON_BOOL(borderless)
	SAVE_JSON_BOOL(resizable)	
	writer.EndObject();
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::OnGui() {
		
	if (ImGui::CollapsingHeader("Window"))
	{
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f)) {
			SDL_SetWindowBrightness(window, brightness);
			SDL_UpdateWindowSurface(window);
		};

		//Refresh rate
		ImGui::Text("Refresh Rate: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%f", App->fps);
		
		if (ImGui::Checkbox("FullScreen", &fullscreen)) {
			fullscreen ? fulldesktop = false : 0;
			if (fullscreen)
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			else
				SDL_SetWindowFullscreen(window, flags);
		}
		ImGui::SameLine();
		if (!fullscreen && ! fulldesktop && ImGui::Checkbox("Resizable", &resizable)) {
			if (resizable)
				SDL_SetWindowResizable(window, SDL_TRUE);
			else
				SDL_SetWindowResizable(window, SDL_FALSE);
		}

		//Second Checkbox Row [BORDERLESS AND FULLDESKTOP]
		if (!fullscreen && !fulldesktop && ImGui::Checkbox("Borderless", &borderless)) {
			if (borderless)
				SDL_SetWindowBordered(window, SDL_FALSE);
			else
				SDL_SetWindowBordered(window, SDL_TRUE);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &fulldesktop)) {
			fulldesktop ? fullscreen = false : 0;
			if (fulldesktop)
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else
				SDL_SetWindowFullscreen(window, flags);
		}
	}
}
