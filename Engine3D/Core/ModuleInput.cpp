#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleImport.h"
#include "GameObject.h"
#include "ModuleTextures.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ComponentMaterial.h"
#include "ImGui/imgui_impl_sdl.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	filePath = nullptr;
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
			{
				keyboard[i] = KEY_DOWN;
				LogInput(i, KEY_DOWN);
			}
			else
			{
				keyboard[i] = KEY_REPEAT;
				LogInput(i, KEY_REPEAT);
			}
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
			{
				keyboard[i] = KEY_UP;
				LogInput(i, KEY_UP);
			}
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_IDLE)
			{
				mouse_buttons[i] = KEY_DOWN;
				LogInput(1000 + i, KEY_DOWN);
				LogInput(1000 + i, KEY_REPEAT);
			}
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
			{
				mouse_buttons[i] = KEY_UP;
				LogInput(1000 + i, KEY_UP);
			}
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch(event.type)
		{
			case SDL_MOUSEWHEEL:
				mouse_z = event.wheel.y;
			break;

			case SDL_MOUSEMOTION:
				mouse_x = event.motion.x / SCREEN_SIZE;
				mouse_y = event.motion.y / SCREEN_SIZE;

				mouse_x_motion = event.motion.xrel / SCREEN_SIZE;
				mouse_y_motion = event.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(event.window.data1, event.window.data2);

				if (event.window.event == SDL_WINDOWEVENT_CLOSE)   // exit game
					App->closeEngine = true;

				break;
			}
			break;

			case SDL_DROPFILE:
			{
				filePath = event.drop.file;
				std::string fileName(filePath);
				if (fileName.substr(fileName.find_last_of(".")) == ".fbx" || fileName.substr(fileName.find_last_of(".")) == ".FBX" || fileName.substr(fileName.find_last_of(".")) == ".OBJ" || fileName.substr(fileName.find_last_of(".")) == ".obj")
				{
					LOG("Path of file dropped will be %s", filePath);
					App->import->LoadGeometry(filePath);
				}
				else if (fileName.substr(fileName.find_last_of(".")) == ".jpg" || fileName.substr(fileName.find_last_of(".")) == ".png" || fileName.substr(fileName.find_last_of(".")) == ".PNG" || fileName.substr(fileName.find_last_of(".")) == ".JPG")
				{
					LOG("Path of file dropped will be %s", filePath);
					std::string realFileName = fileName.substr(fileName.find_last_of("\\") + 1); 					
					if (App->textures->Find(realFileName))
					{
						TextureObject texture = App->textures->Get(realFileName);
						if (App->editor->gameobjectSelected)
						{
							if (ComponentMaterial* material = App->editor->gameobjectSelected->GetComponent<ComponentMaterial>())
							{
								material->SetTexture(texture);
							}

						}
					}
					else
					{
						TextureObject texture = App->textures->Load(realFileName);
						if (App->editor->gameobjectSelected)
						{
							if (ComponentMaterial* material = App->editor->gameobjectSelected->GetComponent<ComponentMaterial>())
							{
								material->SetTexture(texture);
							}

						}
					}
				}
			};
			SDL_free(&filePath);
			break;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::OnGui() {	
	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse Position");

		ImGui::Text("X: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_x);
		ImGui::SameLine();
		ImGui::Text("Y: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_y);

		ImGui::Text("Mouse Movement");

		ImGui::Text("X: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_x_motion);
		ImGui::SameLine();
		ImGui::Text("Y: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_y_motion);

		ImGui::Text("Mouse Wheel");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouse_z);

		ImGui::BeginChild("Input Log");
		ImGui::TextUnformatted(input.begin());
		if (scroll) ImGui::SetScrollHereY(1);
		scroll = false;
		ImGui::EndChild();	
	}
}

void ModuleInput::LogInput(uint key, uint state)
{
	static char entry[512];
	static const char* states[] = { "IDLE", "DOWN", "REPEAT", "UP" };

	if (key < 1000) sprintf_s(entry, 512, "Key: %02u - %s\n", key, states[state]);
	else sprintf_s(entry, 512, "Mouse: %02u - %s\n", key - 1000, states[state]);

	input.appendf(entry);
	scroll = true;
}