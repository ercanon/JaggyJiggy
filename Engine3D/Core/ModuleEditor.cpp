#include "Globals.h"

//Modules
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleImport.h"
#include "ModuleScene.h"
#include "ModuleViewportFrameBuffer.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"

//Tools
#include <string>
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_internal.h"
#include "glew.h"
#include <gl/GL.h>

ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    showDemoWindow = false;
    showAnotherWindow = false;
    showAboutWindow = false;
    showConfWindow = true;
    showAssetsListWindow = true;
    showAssetsWindow = true;

    showConsoleWindow = true;
    showHierarchyWindow = true;
    showInspectorWindow = true;
    showGameWindow = true;
    showSceneWindow = true;
    showTextures = true;
    showLoadScene = false;

    currentColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    gameobjectSelected = nullptr;
    assetselect = nullptr;
    newCam = nullptr;
}


// Destructor
ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init() 
{
    bool ret = true;

    return ret;
}

// Called before render is available
bool ModuleEditor::Start()
{
	bool ret = true;
	
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    sceneWindow |= ImGuiWindowFlags_NoScrollbar;

    // Setup ImGui style by default
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    
    CreateGridBuffer();

    GameObject* newGameObject = App->scene->CreateGameObject("Camera");
    newCam = new ComponentCamera(newGameObject, true);

    assets = new File("Assets");
    assets->path = assets->name;
    assets->Read();

    folderImage = App->textures->Load("Assets/Resources/Format/folder.png");
    App->import->SaveTexture(folderImage);

    pngImage = App->textures->Load("Assets/Resources/Format/png.png");
    App->import->SaveTexture(pngImage);

    jpgImage = App->textures->Load("Assets/Resources/Format/jpg.png");
    App->import->SaveTexture(jpgImage);

    tgaImage = App->textures->Load("Assets/Resources/Format/tga.png");
    App->import->SaveTexture(tgaImage);

    fbxImage = App->textures->Load("Assets/Resources/Format/fbx.png");
    App->import->SaveTexture(fbxImage);

    folderID = folderImage.id;
    pngID = pngImage.id;
    jpgID = jpgImage.id;
    tgaID = tgaImage.id;
    fbxID = fbxImage.id;

    return ret;
}

update_status ModuleEditor::PreUpdate(float dt) 
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();

    return UPDATE_CONTINUE;

}

// PreUpdate: clear buffer
update_status ModuleEditor::Update(float dt)
{
    DrawGrid();
    //Creating MenuBar item as a root for docking windows
    if (DockingRootItem("Viewport", ImGuiWindowFlags_MenuBar)) 
    {
        MenuBar();
        ImGui::End();
    }

    if (gameobjectSelected != nullptr && App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
        gameobjectSelected->parent->RemoveChild(gameobjectSelected);

    assets->child.clear();
    assetsString.clear();
    assets->child.shrink_to_fit();
    assets->Read();
    if (assetselect != nullptr)
    {
        assetselect = assetFile;
        AssetsArray();
    }

    //Update status of each window and shows ImGui elements
    UpdateWindowStatus();

    return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt) 
{

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Rendering
    ImGui::Render();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
        SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }

    ImGui::EndFrame();

    return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

	return true;
}

void ModuleEditor::CreateGridBuffer()
{
    std::vector<float3> vertices;
    std::vector<uint> indices;
    constexpr int slices = 200;
    constexpr float size = 2000.f;
    constexpr float halfSize = size * .5f;
    constexpr float sliceSize = size / static_cast<float>(slices);

   for (int i = 0; i < slices; ++i) 
   {
        const float x = -halfSize + static_cast<float>(i) * sliceSize;
        if (x > 0.01f || x < -0.01f)
        {
            vertices.push_back(float3(x, 0.f, -halfSize));
            vertices.push_back(float3(x, 0.f, halfSize));
            indices.push_back(vertices.size() - 2);
            indices.push_back(vertices.size() - 1);
        }
        const float z = -halfSize + static_cast<float>(i) * sliceSize;
        if (z > 0.01f || z < -0.01f)
        {
            vertices.push_back(float3(-halfSize, 0.f, z));
            vertices.push_back(float3(halfSize, 0.f, z));
            indices.push_back(vertices.size() - 2);
            indices.push_back(vertices.size() - 1);
        }
   }

    glGenVertexArrays(1, &grid.VAO);
    glBindVertexArray(grid.VAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    grid.length = (GLuint)indices.size() * 4;
}

ModuleEditor::Grid::~Grid()
{
    glDeleteBuffers(1, &VAO);
}

void ModuleEditor::DrawGrid()
{
    GLboolean isLightingOn, isDepthTestOn;
    glGetBooleanv(GL_LIGHTING, &isLightingOn);
    glGetBooleanv(GL_DEPTH_TEST, &isDepthTestOn);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glBindVertexArray(grid.VAO);

    glColor3f(.5f, .5f, .5f);
    glDrawElements(GL_LINES, grid.length, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    
    glBegin(GL_LINES);
        //x Axis
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(1000.f, 0.0f, 0.f);
        glColor3f(.2f, 0.f, 0.f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(.2f, 0.f, 0.f);
        glVertex3f(-1000.f, 0.0f, 0.f);
        //z Axis
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.0f, 1000.f);
        glColor3f(0.f, 0.f, .2f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(0.f, 0.f, .2f);
        glVertex3f(0.f, 0.0f, -1000.f);
    glEnd();

    isLightingOn ? glEnable(GL_LIGHTING) : 0;
    !isDepthTestOn ? glDisable(GL_DEPTH_TEST) : 0;

}

void ModuleEditor::About_Window() 
{

    ImGui::Begin("About 3D Engine", &showAboutWindow);

    ImGui::Separator();
    ImGui::Text("JAGGYJIGGY v0.75");
    ImGui::Separator();
    ImGui::BulletText("Link: https://github.com/ercanon/JaggyJiggy \n");

    ImGui::Separator();
    ImGui::Text("AUTORS");
    ImGui::Separator();
    ImGui::BulletText("Alex Gesti: https://github.com/alexgesti \n");
    ImGui::BulletText("Raul Cano: https://github.com/ercanon \n");
    ImGui::BulletText("Pau Motta: https://github.com/paumotta \n");

    ImGui::Separator();
    ImGui::Text("3RD PARTY LIBRARIES");
    ImGui::Separator();
    ImGui::BulletText("SDL v2.0.12");
    ImGui::BulletText("Glew v2.1.0");
    ImGui::BulletText("OpenGL v3.1.0");
    ImGui::BulletText("ImGui v1.78");
    ImGui::BulletText("MathGeoLib v1.5");
    ImGui::BulletText("PhysFS v3.0.2");
    ImGui::BulletText("DevIL v1.7.8");
    ImGui::BulletText("Assimp v3.1.1");

    ImGui::Separator();
    ImGui::Text("LICENSE\n");
    ImGui::Separator();

    ImGui::Text("MIT License\n\n");
    ImGui::Text("Copyright (c) 2021 ercanon, pixelcuak & paumotta");
    ImGui::Text("\n");
    ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy\n\nof this software and associated documentation files (the 'Software'), to deal\n");
    ImGui::Text("in the Software without restriction, including without limitation the rights\n\nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell\n");
    ImGui::Text("copies of the Software, and to permit persons to whom the Software is\n\nfurnished to do so, subject to the following conditions : \n");
    ImGui::Text("\n");
    ImGui::Text("The above copyright notice and this permission notice shall be included in all\n\ncopies or substantial portions of the Software.\n");
    ImGui::Text("\n");
    ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n");
    ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
    ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n");
    ImGui::Text("SOFTWARE.\n");

    ImGui::End();

}

void ModuleEditor::UpdateText(const char* text) 
{
    consoleText.appendf(text);
    scroll = true;
}

bool ModuleEditor::DockingRootItem(char* id, ImGuiWindowFlags winFlags)
{
    //Setting windows as viewport size
    ImGuiViewport* viewport = ImGui::GetWindowViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    //Setting window style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);

    //Viewport window flags just to have a non interactable white space where we can dock the rest of windows
    winFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar;
    winFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

    bool temp = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    temp = ImGui::Begin(id, &temp, winFlags);
    ImGui::PopStyleVar(3);

    BeginDock(id, ImGuiDockNodeFlags_PassthruCentralNode);

    return temp;
}

void ModuleEditor::BeginDock(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size)
{
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) 
    {
        ImGuiID dock = ImGui::GetID(dockSpaceId);
        ImGui::DockSpace(dock, size, dockFlags);
    }
}

void ModuleEditor::MenuBar() 
{
    /* ---- MAIN MENU BAR DOCKED ----*/
    if (ImGui::BeginMainMenuBar()) 
    {
        /* ---- FILE ---- */
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::MenuItem("Save", "Ctrl + S"))
            {
                App->import->SaveScene("Library/Scenes/");
            }
            if (ImGui::MenuItem("Load", "Ctrl + L"))
                showLoadScene = true;
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "(Alt+F4)")) App->closeEngine = true;
            ImGui::EndMenu();
        }

        /* ---- GAMEOBJECTS ---- */
        if (ImGui::BeginMenu("GameObject")) 
        {
            if (ImGui::MenuItem("Create empty GameObject")) 
                App->scene->CreateGameObject();

            if (ImGui::BeginMenu("3D Objects")) 
            {
                /*if (ImGui::MenuItem("Cube")) 
                {
                    GameObject* newGameObject = App->scene->CreateGameObject("Cube");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::CUBE);
                }*/
                if (ImGui::MenuItem("Sphere")) 
                {
                    GameObject* newGameObject = App->scene->CreateGameObject("Sphere");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::SPHERE);
                }
                if (ImGui::MenuItem("Plane")) 
                {
                    GameObject* newGameObject = App->scene->CreateGameObject("Plane");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::PLANE);
                }
                /*if (ImGui::MenuItem("Pyramid")) 
                {
                    GameObject* newGameObject = App->scene->CreateGameObject("Pyramid");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::PYRAMID);
                }*/
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        /* ---- WINDOW ----*/
        if (ImGui::BeginMenu("Window")) 
        {
            if (ImGui::MenuItem("Examples")) showDemoWindow = !showDemoWindow;
            ImGui::Separator();

            if (ImGui::BeginMenu("Workspace Style")) 
            {
                if (ImGui::MenuItem("Dark")) 
                    ImGui::StyleColorsDark();
                if (ImGui::MenuItem("Classic")) 
                    ImGui::StyleColorsClassic();
                if (ImGui::MenuItem("Light")) 
                    ImGui::StyleColorsLight();
                if (ImGui::MenuItem("Custom")) 
                    ImGui::StyleColorsCustom();
                ImGui::EndMenu();
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Hierarchy")) 
                showHierarchyWindow = !showHierarchyWindow;
            if (ImGui::MenuItem("Inspector")) 
                showInspectorWindow = !showInspectorWindow;
            if (ImGui::MenuItem("Scene")) 
                showSceneWindow = !showSceneWindow;
            if (ImGui::MenuItem("Game")) 
                showGameWindow = !showGameWindow;
            if (ImGui::MenuItem("Console")) 
                showConsoleWindow = !showConsoleWindow;
            if (ImGui::MenuItem("Textures"))
                showTextures = !showTextures;

            ImGui::Separator();
            if (ImGui::MenuItem("Configuration")) 
                showConfWindow = !showConfWindow;
            

            ImGui::EndMenu();
        }

        /* ---- HELP ----*/
        if (ImGui::BeginMenu("Help")) 
        {
            if (ImGui::MenuItem("About")) 
                showAboutWindow = !showAboutWindow;
            ImGui::EndMenu();
        }

        /* ---- TIME MANAGMENT ----*/
        ImGui::Separator();
        if (ImGui::MenuItem("Play"))
        {
            play = !play;
            // Save
            if (play == false)
            {

            }
            // Load
            if (play == true)
            {

            }
        }
        if (ImGui::MenuItem("Pause")) if (play == false) pause = !pause;
        if (ImGui::MenuItem("Stop"))
        {
            if (play == false)
            {
                // Load


                play = true;
                pause = true;
            }
        }
    }

    ImGui::EndMainMenuBar();
}

void ModuleEditor::UpdateWindowStatus() 
{
    //Demo
    if (showDemoWindow) 
        ImGui::ShowDemoWindow(&showDemoWindow);

    //About info
    if (showAboutWindow)  
        About_Window();

    //Config
    if (showConfWindow) 
    {
        ImGui::Begin("Configuration", &showConfWindow);        
        App->OnGui();
        ImGui::End();
    }

    //Textures
    if (showTextures)
    {
        ImGui::Begin("Textures", &showTextures);
        for (auto& t : App->textures->textures)
        {
            ImGui::Image((ImTextureID)t.second.id, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();
    }
        
    //Console
    if (showConsoleWindow) 
    {
        ImGui::Begin("Console", &showConsoleWindow);
        if (pause == true) ImGui::TextUnformatted(consoleText.begin(), consoleText.end());
        else ImGui::Text("---- Game Paused ----");
        if (scroll) ImGui::SetScrollHere(1.0f);
        scroll = false;
        ImGui::End();
    }

    //Assets List
    if (showAssetsListWindow)
    {
        ImGui::Begin("Assets List", &showAssetsListWindow);

        static char buf[32] = " "; ImGui::InputText(" ", buf, 32);
        if (ImGui::Button("Create folder", ImVec2(120, 0)))
        {
            std::string path;
            if (assetselect != nullptr)
            {
                path = assetselect->path.c_str() + std::string("/") + std::string(buf);
                App->fileSystem->CreateDir(path.c_str());
            }
            else
            {
                path = assets->path.c_str() + std::string("/") + std::string(buf);
                App->fileSystem->CreateDir(path.c_str());
            }
        }
        ImGui::Separator();

        std::stack<File*> F;
        std::stack<uint> indents;

        F.push(assets);
        indents.push(0);

        File* file;

        while (!F.empty())
        {
            file = F.top();
            uint max = indents.top();
            F.pop();
            indents.pop();

            ImGuiTreeNodeFlags nodeFlags = 0;
            if (file->selected)  nodeFlags |= ImGuiTreeNodeFlags_Selected;
            if (file->child.size() == 0)   nodeFlags |= ImGuiTreeNodeFlags_Leaf;

            for (uint i = 0; i < max; ++i)
            {
                ImGui::Indent();
            }

            if (ImGui::TreeNodeEx(file->name.c_str(), nodeFlags))
            {
                /*if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("DragDropHierarchy", &assetFile, sizeof(File*), ImGuiCond_Once);
                    ImGui::Text("%s", assetFile->name.c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(File*));
                        File* droppedGo = (File*)*(const int*)payload->Data;
                        if (droppedGo)
                        {
                            // Remove Child
                            //auto it = std::find(droppedGo->child.begin(), droppedGo->child.end(), droppedGo->child);

                            //if (it != droppedGo->child.end()) droppedGo->child.erase(it);
                            
                            // Attach Child
                            assetFile->child.push_back(droppedGo);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }*/

                if (ImGui::IsItemClicked())
                {
                    if (gameobjectSelected != nullptr) gameobjectSelected = false;
                    gameobjectSelected = nullptr;

                    assetselect ? assetselect->selected = !assetselect->selected : 0;
                    assetselect = file;
                    assetselect->selected = !assetselect->selected;

                    if (assetselect->selected)
                    {
                        assetsString.clear();
                        AssetsArray();
                    }
                }

                if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
                    if (assetselect != nullptr) CleanUpObject();

                for (File* child : file->child)
                {
                    F.push(child);
                    indents.push(max + 1);
                }

                for (uint i = 0; i < max; ++i) ImGui::Unindent();

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    //Assets
    if (showAssetsWindow)
    {
        ImGui::Begin("Assets", &showAssetsWindow);

        if (assetselect != nullptr)
        {
            std::stack<File*> F;
            F.push(assetselect);

            while (!F.empty())
            {
                assetFile = F.top();
                F.pop();
                ImGuiTreeNodeFlags nodeFlags = 0;

                if (assetFile->selected && assetselect)
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                if (assetFile->child.size() == 0)
                    nodeFlags |= ImGuiTreeNodeFlags_Leaf;

                if (assetFile->selected && assetselect)
                {
                    if (assetsString.size() > 0)
                    {
                        ImGui::Columns(assetsString.size(), NULL, false);
                        for (int i = 0; i < assetsString.size(); i++)
                        {
                            if (!App->fileSystem->HasExtension(assetsString.at(i).c_str()))
                            {
                                DrawID(folderID, assetsString.at(i).c_str(), i);
                            }
                            if (assetsString.size() > 0 && i < assetsString.size())
                            {
                                std::string str = assetselect->path + assetsString.at(i);

                                if (App->fileSystem->HasExtension(str.c_str(), "png")) DrawID(pngID, assetsString.at(i).c_str(), i);
                                if (App->fileSystem->HasExtension(str.c_str(), "jpg")) DrawID(jpgID, assetsString.at(i).c_str(), i);
                                if (App->fileSystem->HasExtension(str.c_str(), "tga")) DrawID(tgaID, assetsString.at(i).c_str(), i);
                                if (App->fileSystem->HasExtension(str.c_str(), "fbx")) DrawID(fbxID, assetsString.at(i).c_str(), i);
                            }
                            ImGui::NextColumn();
                        }
                    }
                }
            }
        }

        ImGui::End();
    }

    //Inspector
    if (showInspectorWindow) 
    {
        ImGui::Begin("Inspector", &showInspectorWindow);
        //Only shows info if any gameobject selected
        if (gameobjectSelected != nullptr) 
            InspectorGameObject(); 

        ImGui::End();
    }

    //Hierarchy
    if (showHierarchyWindow) 
    {
        ImGui::Begin("Hierarchy", &showHierarchyWindow);
        
        S.push(App->scene->root);
        indents.push(0);

        while (!S.empty())
        {
            go = S.top();
            uint indentsAmount = indents.top();
            S.pop();
            indents.pop();

            ImGuiTreeNodeFlags nodeFlags = 0;
            if (go->isSelected)
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            if (go->children.size() == 0)
                nodeFlags |= ImGuiTreeNodeFlags_Leaf; 
            for (uint i = 0; i < indentsAmount; ++i)
            {
                ImGui::Indent();
            }

            if (ImGui::TreeNodeEx(go->name.c_str(), nodeFlags)) 
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("DragDropHierarchy", &go, sizeof(GameObject*), ImGuiCond_Once);
                    ImGui::Text("%s", go->name.c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(GameObject*));
                        GameObject* droppedGo = (GameObject*)*(const int*)payload->Data;
                        if (droppedGo)
                        {
                            droppedGo->parent->RemoveChild(droppedGo);
                            go->AttachChild(droppedGo);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::IsItemClicked()) 
                {
                    if (assetselect != nullptr) assetselect->selected = false;
                    assetselect = nullptr;

                    gameobjectSelected ? gameobjectSelected->isSelected = !gameobjectSelected->isSelected : 0;
                    gameobjectSelected = go;
                    gameobjectSelected->isSelected = !gameobjectSelected->isSelected;
                    if (gameobjectSelected->isSelected)
                    {
                        LOG("GameObject selected name: %s", gameobjectSelected->name.c_str());
                    }
                    else
                    {
                        LOG("GameObject unselected name: %s", gameobjectSelected->name.c_str());
                    }
                }
                for (GameObject* child : go->children)
                {
                    S.push(child);
                    indents.push(indentsAmount + 1);
                }

                for (uint i = 0; i < indentsAmount; ++i)
                {
                    ImGui::Unindent();
                }

                ImGui::TreePop();
            }       
        }
        ImGui::End();
    }

    if (showGameWindow) 
    {
        ImGui::Begin("Game", &showGameWindow, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);

        if (newCam != nullptr)
        {
            ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
            if (viewportSize.x != lastViewportSize2.x || viewportSize.y != lastViewportSize2.y)
            {
                newCam->aspectRatio = viewportSize.x / viewportSize.y;
                newCam->RecalculateProjection();
            }

            lastViewportSize2 = viewportSize;
            ImGui::Image((ImTextureID)App->viewportBufferGame->texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();
    }

    if (showSceneWindow) 
    {
        ImGui::Begin("Scene", &showSceneWindow, ImGuiWindowFlags_NoScrollbar);

        if (App->editor->gameobjectSelected != nullptr)
        {
            App->camera->cornerPos = ImGui::GetWindowPos();
            App->camera->size = ImGui::GetContentRegionMax();
        }

        ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
        if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
        {
            App->camera->aspectRatio = viewportSize.x / viewportSize.y;
            App->camera->RecalculateProjection();
        }

        lastViewportSize = viewportSize;
        ImGui::Image((ImTextureID)App->viewportBufferScene->texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsWindowFocused()) App->camera->isMouseFocused = true;
        else App->camera->isMouseFocused = false;
        
        // Mouse clicking ----------------
        if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) App->camera->IsMouseClicked();

        // Gizmo -------------------------
        //if (gameobjectSelected != nullptr) App->camera->EditTransform();

        ImGui::End();
    }

    if (showLoadScene)
    {
        ImGui::OpenPopup("Load File");
        if (ImGui::BeginPopupModal("Load File", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure you want to open a new scene?\n\nWARNING:\nYou will delete the actual scene!");

            if (ImGui::Button("Ok", ImVec2(50, 20)))
            {
                App->textures->CleanUp();
                App->scene->CleanUp();
                App->import->LoadScene("Library/Scenes/scene.jiy");
                showLoadScene = false;
            }
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(50, 20)))
                showLoadScene = false;

            ImGui::EndPopup();
        }
    }
}

void ModuleEditor::AssetsArray()
{
    std::stack<File*> F;
    F.push(assetselect);

    while (!F.empty())
    {
        assetFile = F.top();
        F.pop();

        if (!App->fileSystem->HasExtension(assetFile->name.c_str()))
        {
            for (int i = assetFile->child.size() - 1; i >= 0; i--)
            {
                std::string str = assetFile->child.at(i)->name;
                assetsString.push_back(str);
            }
        }

        for (int i = assetFile->files.size() - 1; i >= 0; i--)
        {
            if (App->fileSystem->HasExtension(assetFile->files.at(i).c_str()))
            {
                std::string str = assetFile->files.at(i);
                assetsString.push_back(str);
            }
        }
    }
}

void ModuleEditor::DrawID(uint id, const char* text, int numID)
{
    ImGui::PushID(numID);

    if (ImGui::ImageButton((ImTextureID)id, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0), 1))
    {
        std::string str = assetselect->path + std::string(text);
        if (!App->fileSystem->HasExtension(str.c_str()))
        {
            assetselect->selected = false;
            for (uint i = 0; i < assetselect->child.size(); i++)
            {
                if (assetselect->child.at(i)->name == text)
                {
                    assetsString.clear();
                    assetselect = assetselect->child.at(i);
                    assetFile = assetselect;
                    assetselect->selected = true;
                    AssetsArray();
                }
            }
            LOG("Open folder: %s", assetselect->path.c_str());
        }
        else
        {
            ShellExecute(NULL, "open", App->fileSystem->NormalizePath(str.c_str()).c_str(), NULL, NULL, SW_SHOWNORMAL);
            LOG("File Path: %s", str.c_str());

            if (App->fileSystem->HasExtension(str.c_str(), "fbx")) App->import->LoadGeometry(str.c_str());
            if (App->fileSystem->HasExtension(str.c_str(), "jpg") ||
                App->fileSystem->HasExtension(str.c_str(), "png") ||
                App->fileSystem->HasExtension(str.c_str(), "tga"))
            {
                TextureObject texture = App->textures->Load(str.c_str());
                App->import->SaveTexture(texture);
            }
        }
    }
    ImGui::Text(text);

    ImGui::PopID();
}

void ModuleEditor::CleanUpObject()
{
   if (assetselect->child.size() > 0)
    {
        for (int i = 0; i < assetselect->files.size(); i++)
        {
            std::string file = assetselect->path + std::string(assetselect->files.at(i));
            App->fileSystem->DeleteDir(file.c_str());
        }
    }
    App->fileSystem->DeleteDir(assetselect->path.c_str());
    assetselect = nullptr;
}

void ModuleEditor::InspectorGameObject() 
{
    if (gameobjectSelected)
        gameobjectSelected->OnGui();
}

