#include "PhobosPCH.h"
#include "EditorLayer.h"
#include "EditorGUI.h"
#include "Ares/ImGui/ImGuizmo.h"
#include <filesystem>
#include "MaterialEditor.h"
#include "EditorUtility.h"


#define _2D 0

namespace Ares
{

    EditorLayer::EditorLayer()
        : Layer("Phobos Editor"), 
        m_CameraController(1280.0f / 720.0f),
        m_EditorCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
    {
    }
    void EditorLayer::OnAttach()
    {
        m_FileSystemWatcher.Watch();
        EditorGUI::InitializeGUIColors();
        memset(m_FrameTimeGraph, 0, sizeof(float) * 100);

#if _2D
        // create a frame buffer
        FrameBufferSpecs fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        fbSpec.Format = FramebufferFormat::RGBA8;
        m_FrameBuffer = Ares::FrameBuffer::Create(fbSpec);

        // create scene
        m_ActiveScene = CreateRef<Scene>();
        
        // create sprite entity to draw
        {
            m_SquareEntity = m_ActiveScene->CreateEntity("Custom Entity");
            // add a sprite renderer component
            SpriteRendererComponent& spriteRenderer = m_SquareEntity.AddComponent<SpriteRendererComponent>();
            Ref<Texture2D> spriteSheet = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png", FilterType::Trilinear, true);
            spriteRenderer.Texture = spriteSheet;
        }

        // create a camera entity
        {
            m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
            // add a camera component
            m_CameraEntity.AddComponent<CameraComponent>();
        }

#else
        // Editor
        m_PlayButtonTex = EditorResources::GetTexture("play.png");

        m_AssetManagerPanel = CreateScope<AssetManagerPanel>();



        auto environment = Environment::Load(
            "C:\\Users\\Andres\\Desktop\\DevProjects\\Hazel\\Hazel-dev\\Hazelnut\\assets\\env\\venice_dawn_1_4k.hdr"
        );

        m_EditorScene = CreateRef<Scene>("Editor Scene");
        UpdateWindowTitle("Editor Scene");

        auto skyboxShader = Shader::Find("Assets/Shaders/CubemapSkybox.glsl");
        auto skyboxMaterial = CreateRef<Material>(skyboxShader);

        skyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);

        m_EditorScene->SetSkyboxMaterial(skyboxMaterial);
        m_EditorScene->SetEnvironment(environment);

        m_SceneHierarchyPanel = CreateScope<SceneHierarchyPanel>(m_EditorScene);
        
        m_SceneHierarchyPanel->SetSelectionChangedCallback(std::bind(&EditorLayer::SelectEntity, this, std::placeholders::_1));
        m_SceneHierarchyPanel->SetEntityDeletedCallback(std::bind(&EditorLayer::OnEntityDeleted, this, std::placeholders::_1));
        
        Ref<Material> m_MeshBaseMaterial = CreateRef<Material>(Shader::Find("Assets/Shaders/Standard.glsl"));

        Entity gunEntity = m_EditorScene->CreateEntity("Gun");
        MeshRendererComponent* mr = gunEntity.AddComponent<MeshRendererComponent>();

        std::vector<Ref<Material>> loadedMaterials;
        mr->Mesh = CreateRef<Mesh>(
            "C:\\Users\\Andres\\Desktop\\DevProjects\\Hazel\\Hazel-dev\\Hazelnut\\assets\\models\\m1911\\M1911Materials.fbx",
            loadedMaterials
        );
        mr->Materials = loadedMaterials;
        
        m_MeshMaterials = loadedMaterials;

        if (!loadedMaterials.size())
        {
            ARES_WARN("Couldnt Find Materials... setting custom");
            mr->Materials = { m_MeshBaseMaterial };
            
            m_MeshMaterials = mr->Materials;
        }

        gunEntity.Transform() = glm::scale(
            glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 2)),
            glm::vec3(15.0f)
        );
        
        auto sphereMesh = CreateRef<Mesh>(PrimitiveMeshType::Cube);

        float spread = 1;
        uint8_t i = 0;
        for (int y = -2; y <= 2; y++)
        {
            float metalness = (float)(y + 2) / 4.0f;
            for (int x = -2; x <= 2; x++)
            {
                float roughness = 1.0f - ((float)(x + 2) / 4.0f);

                Entity sphereEntity = m_EditorScene->CreateEntity("Sphere M:" + std::to_string(metalness) + " / R:" + std::to_string(roughness));

                MeshRendererComponent* mrC = sphereEntity.AddComponent<MeshRendererComponent>();
                mrC->Mesh = sphereMesh;

                Ref<Material> m = CreateRef<Material>(Shader::Find("Assets/Shaders/Standard.glsl"));
                mrC->Materials = { m };
                m->SetValue("u_Metalness", metalness);
                m->SetValue("u_Roughness", roughness);

                sphereEntity.Transform() = glm::translate(
                    glm::mat4(1.0f), 
                    glm::vec3(x * spread, y * spread, 0)
                );
            }
        }

        auto& light = m_EditorScene->GetLight();
        light.Direction = { -0.5f, -0.5f, 0.5f };
        light.Radiance = { 1.0f, 1.0f, 1.0f };

        // SceneSerializer serializer(m_ActiveScene);
        // serializer.Deserialize("Scene.yaml");
#endif
    }

    void EditorLayer::OnDetach()
    {
        
    }

#if _2D
    void EditorLayer::OnUpdate()
    {
        
        // Resize
        if (m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y))
        {
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }
        
        if (m_ViewportFocused)
            m_CameraController.OnUpdate();

        // render
        m_FrameBuffer->Bind();

        Renderer::Clear(.1f, .1f, .1f, 1);
            
        // update scene (starts and ends renderer2D scene)
        m_ActiveScene->OnUpdate();


        m_FrameBuffer->Unbind();
    }
#else


    void EditorLayer::OnScenePlay()
    {
        m_SelectionContext.clear();

        m_SceneState = SceneState::Play;

        m_RuntimeScene = CreateRef<Scene>("Runtime Scene");
        m_EditorScene->CopyTo(m_RuntimeScene);

        m_RuntimeScene->OnRuntimeStart();
        m_SceneHierarchyPanel->SetContext(m_RuntimeScene);
    }
    void EditorLayer::OnSceneStop()
    {
        m_RuntimeScene->OnRuntimeStop();
        m_SceneState = SceneState::Edit;

        // Unload runtime scene
        m_RuntimeScene = nullptr;

        m_SelectionContext.clear();
        m_SceneHierarchyPanel->SetContext(m_EditorScene);
    }

    void EditorLayer::UpdateWindowTitle(const std::string& sceneName)
    {
        std::string title = sceneName + " - Phobos - " + Application::GetPlatformName() + " (" + Application::GetConfigurationName() + ")";
        Application::Get().GetWindow().SetTitle(title);
    }

    void EditorLayer::OnUpdate()
    {        
        

        if (Input::GetKeyDown(KeyCode::Z))
        {
            bool controlHeld = Input::GetKey(KeyCode::LeftControl) || Input::GetKey(KeyCode::RightControl);
            if (controlHeld)
            {
                if (Input::GetKey(KeyCode::LeftShift) || Input::GetKey(KeyCode::RightShift))
                {
                    EditorUtility::TriggerRedo();
                }
                else
                {
                    EditorUtility::TriggerUndo();
                }
            }
        }


        switch (m_SceneState)
        {
        case SceneState::Edit:
        {
            //if (m_ViewportPanelFocused)
                m_EditorCamera.Update();

            m_EditorScene->OnRenderEditor(m_EditorCamera);

            if (m_DrawOnTopBoundingBoxes)
            {
                Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
                auto viewProj = m_EditorCamera.GetViewProjection();
                Renderer2D::BeginScene(viewProj, false);
                // TODO: Renderer::DrawAABB(m_MeshEntity.GetComponent<MeshComponent>(), m_MeshEntity.GetComponent<TransformComponent>());
                Renderer2D::EndScene();
                Renderer::EndRenderPass();
            }

            if (m_SelectionContext.size() && false)
            {
                auto& selection = m_SelectionContext[0];

                if (selection.Mesh && selection.Entity.HasComponent<MeshRendererComponent>())
                {
                    Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
                    auto viewProj = m_EditorCamera.GetViewProjection();
                    Renderer2D::BeginScene(viewProj, false);
                    glm::vec4 color = (m_SelectionMode == SelectionMode::Entity) ? glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f } : glm::vec4{ 0.2f, 0.9f, 0.2f, 1.0f };
                    Renderer::DrawAABB(selection.Mesh->BoundingBox, selection.Entity.GetComponent<TransformComponent>()->Transform * selection.Mesh->Transform, color);
                    Renderer2D::EndScene();
                    Renderer::EndRenderPass();
                }
            }
            
            break;
        }
        case SceneState::Play:
        {
            if (m_ViewportPanelFocused)
                m_EditorCamera.Update();

            m_RuntimeScene->OnUpdate();
            m_RuntimeScene->OnRenderRuntime();
            break;
        }
        case SceneState::Pause:
        {
            if (m_ViewportPanelFocused)
                m_EditorCamera.Update();

            m_RuntimeScene->OnRenderRuntime();
            break;
        }
        }
    }

    void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
    {
        SceneRenderer::GetOptions().ShowBoundingBoxes = show && !onTop;
        m_DrawOnTopBoundingBoxes = show && onTop;
    }

    void EditorLayer::SelectEntity(Entity entity)
    {
        SelectedSubmesh selection;
        if (entity.HasComponent<MeshRendererComponent>())
        {
            selection.Mesh = &entity.GetComponent<MeshRendererComponent>()->Mesh->GetSubmeshes()[0];
        }
        selection.Entity = entity;
        m_SelectionContext.clear();
        m_SelectionContext.push_back(selection);

        m_EditorScene->SetSelectedEntity(entity);
    }


#endif

    void EditorLayer::DrawToolbar()
    {
    /*
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::Begin("Toolbar");// , 0, ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton);
        if (m_SceneState == SceneState::Edit)
        {
            if (ImGui::ImageButton((ImTextureID)(intptr_t)(m_PlayButtonTex->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(0.9f, 0.9f, 0.9f, 1.0f)))
            {
                OnScenePlay();
            }
        }
        else if (m_SceneState == SceneState::Play)
        {
            if (ImGui::ImageButton((ImTextureID)(intptr_t)(m_PlayButtonTex->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(1.0f, 1.0f, 1.0f, 0.2f)))
            {
                OnSceneStop();
            }
        }

        ImGui::End();

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    */
    }


    static bool builtToolbar = false;


    /*

    void DockSpaceUI()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, toolbarSize));
        ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags window_flags = 0
            | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("Master DockSpace", NULL, window_flags);
        ImGuiID dockMain = ImGui::GetID("MyDockspace");

        // Save off menu bar height for later.
        menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();

        ImGui::DockSpace(dockMain);
        ImGui::End();
        ImGui::PopStyleVar(3);
    }
    */

    static bool showStats = true;

    static float menuBarHeight = 0;
    const float toolbarSize = 56.0f;
    void EditorLayer::ToolbarUI()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        float toolbarWidth = viewport->Size.x;
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));// +menuBarHeight * 2));
        ImGui::SetNextWindowSize(ImVec2(toolbarWidth, toolbarSize + menuBarHeight));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = 0
            | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove 
            | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings
        ;


        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

        ImGui::Begin("TOOLBAR", NULL, window_flags);
        
        ImGui::PopStyleVar();
        
        DrawMenu();
        menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();

        ImTextureID buttonTex = (ImTextureID)(intptr_t)(m_PlayButtonTex->GetRendererID());
        ImVec2 buttonSize = ImVec2(37, 37);

        ImVec4 backGroundActive = ImGui::GetStyle().Colors[ImGuiCol_TabActive];
        ImVec4 activeTint = ImGui::GetStyle().Colors[ImGuiCol_Text];

        float startx = toolbarWidth * .5f - buttonSize.x * .5f;
        
        ImGui::SetCursorPos(ImVec2(startx, ImGui::GetCursorPos().y));
        if (ImGui::ImageButton(buttonTex, buttonSize, ImVec2(0, 0), ImVec2(1, 1), 0, m_SceneState == SceneState::Edit ? ImVec4(0, 0, 0, 0) : backGroundActive, activeTint))
        {
            if (m_SceneState == SceneState::Edit)
            {
                OnScenePlay();
            }
            else
            {
                OnSceneStop();
            }
        }



        //ImVec2 s = ImGui::CalcTextSize("Show Stats");
        float buttonWidth = 100;
        ImGui::SameLine((toolbarWidth - 10) - buttonWidth);
        ImGui::PushStyleColor(ImGuiCol_Button, showStats ? backGroundActive : ImGui::GetStyle().Colors[ImGuiCol_Button]);
        
        
        
        if (ImGui::Button("Show Stats", ImVec2(buttonWidth, -1)))
        {
            showStats = !showStats;
        }
        ImGui::PopStyleColor();
        
        ImGui::End();
    }

    static bool dockspaceOpen = true;


    // to do: add edit
    void EditorLayer::DrawMenu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {

                if (ImGui::MenuItem("New Scene"))
                {

                }
                if (ImGui::MenuItem("Open Scene..."))
                {
                    auto& app = Application::Get();
                    std::string filepath = app.OpenFile("Hazel Scene (*.hsc)\0*.hsc\0");
                    if (!filepath.empty())
                    {
                        Ref<Scene> newScene = CreateRef<Scene>();
                        SceneSerializer serializer(newScene);
                        serializer.Deserialize(filepath);
                        m_EditorScene = newScene;
                        std::filesystem::path path = filepath;
                        UpdateWindowTitle(path.filename().string());
                        m_SceneHierarchyPanel->SetContext(m_EditorScene);

                        m_EditorScene->SetSelectedEntity({});
                        m_SelectionContext.clear();
                    }
                }
                if (ImGui::MenuItem("Save Scene..."))
                {
                    auto& app = Application::Get();
                    std::string filepath = app.SaveFile("Hazel Scene (*.hsc)\0*.hsc\0");
                    SceneSerializer serializer(m_EditorScene);
                    serializer.Serialize(filepath);

                    std::filesystem::path path = filepath;
                    UpdateWindowTitle(path.filename().string());
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                    dockspaceOpen = false;

                    Ares::Application::Get().Close();
                }

                ImGui::EndMenu();
            }
            //if (ImGui::BeginMenu("Docking"))
            //{
            //    // Disabling fullscreen would allow the window to be moved to the front of other windows, 
            //    // which we can't undo at the moment without finer window depth/z control.
            //    //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            //    if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 
            //        dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            //    if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  
            //        dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            //    if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                
            //        dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            //    if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          
            //        dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            //    ImGui::Separator();
            //    if (ImGui::MenuItem("Close DockSpace", NULL, false, dockspaceOpen != NULL))
            //        dockspaceOpen = false;

            //    ImGui::EndMenu();

            //}
            /*EditorGUI::ShowTooltip(
                "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n\n"
                " > if io.ConfigDockingWithShift==false (default):" "\n"
                "   drag windows from title bar to dock" "\n"
                " > if io.ConfigDockingWithShift==true:" "\n"
                "   drag windows from anywhere and hold Shift to dock" "\n\n"
                "This demo app has nothing to do with it!" "\n\n"
                "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)." "\n\n"
                "ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame." "\n\n"
                "(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
            );*/


            ImGui::EndMenuBar();
        }
    }



        
    void EditorLayer::OnImGuiDraw()
    {

        ARES_PROFILE_FUNCTION();

        ToolbarUI();






















        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        //ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;



        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();


            //ImGui::SetNextWindowPos(viewport->GetWorkPos());
            //ImGui::SetNextWindowSize(viewport->GetWorkSize());

            ImVec2 wPos = viewport->GetWorkPos();
            ImVec2 wSize = viewport->GetWorkSize();
            ImGui::SetNextWindowPos(ImVec2(wPos.x, wPos.y + (toolbarSize + menuBarHeight)));
            ImGui::SetNextWindowSize(ImVec2(wSize.x, wSize.y - (toolbarSize + menuBarHeight)));


            ImGui::SetNextWindowViewport(viewport->ID);



            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        /*if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;*/

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


        ImGui::Begin("DockSpace Demo", NULL, window_flags);

        ImGui::PopStyleVar();
        
        //DrawMenu();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");


            // Save off menu bar height for later.
            //menuBarHeight = ImGui::GetCurrentWindow()->MenuBarHeight();


            /*


            // NEW
            if (!ImGui::DockBuilderGetNode(dockspace_id)) {
                ImGui::DockBuilderRemoveNode(dockspace_id);

                //ImGuiDockNodeFlags_Dockspace
                //ImGuiDockNodeFlags_None
                //ImGuiDockNodeFlags_PassthruCentralNode
                ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);// ImGuiDockNodeFlags_None);

                ImGuiID dock_main_id = dockspace_id;

                auto [w, h] = Application::Get().GetWindow().GetSize();
                ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(w,h));
                ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.05f, nullptr, &dock_main_id);




                /
                ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
                ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
                ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
                ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);
                /

                ImGui::DockBuilderDockWindow("Toolbar", dock_up_id);


                /
                ImGui::DockBuilderDockWindow("Actions", dock_up_id);
                ImGui::DockBuilderDockWindow("Hierarchy", dock_right_id);
                ImGui::DockBuilderDockWindow("Inspector", dock_left_id);
                ImGui::DockBuilderDockWindow("Console", dock_down_id);
                ImGui::DockBuilderDockWindow("Project", dock_down_right_id);
                ImGui::DockBuilderDockWindow("Scene", dock_main_id);
                /

                // Disable tab bar for custom toolbar
                ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_up_id);
                node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
                node->LocalFlags |= ImGuiDockNodeFlags_NoWindowMenuButton;
                node->LocalFlags |= ImGuiDockNodeFlags_NoCloseButton;
                node->LocalFlags |= ImGuiDockNodeFlags_NoResize;

                ImGui::DockBuilderFinish(dock_main_id);
            }
            // NEW

            */











            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            // only once...

            //if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
            /*
            if (!builtToolbar)
            {
                //ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
                //ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
                //ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(1,1));
                ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
                ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.20f, NULL, &dock_main_id);
                //ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

                //ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
                //ImGui::DockBuilderDockWindow("Properties", dock_id_prop);
                //ImGui::DockBuilderDockWindow("Mesh", dock_id_prop);
                ImGui::DockBuilderDockWindow("Toolbar", dock_id_prop);
                //ImGui::DockBuilderFinish(dockspace_id);

                builtToolbar = true;
            }
            */
        }

        //DrawMenu();

        //DrawToolbar();

        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        EditorGUI::DrawEditorColorPickers();


        ImGui::Begin("Renderer Performance:");
        {
            /*
            float frameTimeMS = (float)(Time::GetDeltaTime() * 1000.0);

            m_FrameTimeGraph[values_offset] = frameTimeMS;
            values_offset = (values_offset + 1) % 100;

            ImGui::PlotLines("##Frametime", m_FrameTimeGraph, 100, values_offset, "Frametime (ms)", 0.0f, 66.6f, ImVec2(0, 100));
            ImGui::Text("Frametime: %.2fms", frameTimeMS);
            ImGui::Text("FPS: %d", Time::GetFPS());

            auto& caps = RendererAPI::GetCapabilities();
            ImGui::Text("Vendor: %s", caps.Vendor.c_str());
            ImGui::Text("Renderer: %s", caps.Renderer.c_str());
            ImGui::Text("Version: %s", caps.Version.c_str());

            */
            ImGui::End();
        }

#if _2D
        ImGui::Begin("Stats");
        {
            auto stats = Ares::Renderer2D::GetStats();
            ImGui::Text("Renderer2D Stats:");
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quads: %d", stats.QuadCount);
            ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
            ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());

            if (ImGui::DragInt("Max Quads Per Draw", &m_MaxQuadsPerDraw, 10, 0, 100000))
            {
                Renderer2D::SetMaxQuadsPerDraw(m_MaxQuadsPerDraw);
            }
            ImGui::SliderInt("Num Sprites", &m_NumberOfSprites, 0, 1000);

            if (m_SquareEntity)
            {
                ImGui::Separator();

                auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
                ImGui::Text("%s", tag.c_str());

                SpriteRendererComponent& spriteRenderer = m_SquareEntity.GetComponent<SpriteRendererComponent>();

                auto& squareColor = spriteRenderer.Color;

                ImGui::ColorEdit4("Sprite Color", glm::value_ptr(squareColor));

                if (ImGui::SliderInt2("Sprite Sheet Coords", glm::value_ptr(m_SpriteSheetCoord), 0, 20))
                    spriteRenderer.SetSpriteSheetCoords(m_SpriteSheetCoord, { 128, 128 }, m_SpriteSize);

                if (ImGui::SliderInt2("Sprite Size", glm::value_ptr(m_SpriteSize), 1, 3))
                    spriteRenderer.SetSpriteSheetCoords(m_SpriteSheetCoord, { 128, 128 }, m_SpriteSize);


                CameraComponent& cameraComponent = m_CameraEntity.GetComponent<CameraComponent>();
                float orthoSize = cameraComponent.Camera.GetOrthographicSize();

                if (ImGui::SliderFloat("Camera Ortho Size", &orthoSize, .001f, 10))
                {
                    cameraComponent.Camera.SetOrthographicSize(orthoSize);
                }

                ImGui::Separator();
            }

            ImGui::End();
        }

#else

        // Editor Panel ------------------------------------------------------------------------------
        ImGui::Begin("Settings");
        {

        ImGui::Begin("Environment");
        {

            if (ImGui::Button("Load Environment Map"))
            {
                std::string filename = Application::Get().OpenFile("*.hdr");
                if (filename != "")
                    m_EditorScene->SetEnvironment(Environment::Load(filename));
            }

            ImGui::Columns(2);
            ImGui::AlignTextToFramePadding();

            auto& light = m_EditorScene->GetLight();

            EditorGUI::Vec3Field("Light Direction", light.Direction);
            EditorGUI::Color3Field("Light Radiance", light.Radiance);
            EditorGUI::FloatSliderField("Light Multiplier", light.Multiplier, 0.0f, 5.0f);
            EditorGUI::FloatSliderField("Exposure", m_EditorScene->GetExposure(), 0.0f, 5.0f);

            if (EditorGUI::ToggleField("Show Bounding Boxes", m_UIShowBoundingBoxes))
                ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
            if (m_UIShowBoundingBoxes && EditorGUI::ToggleField("On Top", m_UIShowBoundingBoxesOnTop))
                ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);

            char* label = m_SelectionMode == SelectionMode::Entity ? "Entity" : "Mesh";
            if (ImGui::Button(label))
            {
                m_SelectionMode = m_SelectionMode == SelectionMode::Entity ? SelectionMode::SubMesh : SelectionMode::Entity;
            }

            ImGui::Columns(1);

            ImGui::End();
        }

        /* Asset Browser ImGUI Contents Starts Here --------------------------------------------------------------------- */

        m_AssetManagerPanel->RenderAssetWindow();

        /* Asset Browser ImGUI Contents Ends Here ----------------------------------------------------------------------- */

        uint32_t i = 0;
        for (auto material : m_MeshMaterials)
        {
            ImGuiTreeNodeFlags node_flags = (i == 0 ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

            bool opened = ImGui::TreeNodeEx((void*)i, node_flags, material->GetName().c_str());
            if (opened)
            {
                MaterialEditor::DrawMaterial(material);
                ImGui::TreePop();
            }

            i++;
        }

        ImGui::Separator();


        if (ImGui::TreeNode("Shaders"))
        {
            auto& shaders = Shader::s_AllShaders;
            for (auto& shader : shaders)
            {
                if (ImGui::TreeNode(shader->GetName().c_str()))
                {
                    std::string buttonName = "Reload##" + shader->GetName();
                    if (ImGui::Button(buttonName.c_str()))
                        shader->Reload();
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        ImGui::End();
    }

        // ImGui::ShowDemoWindow();

        

#endif

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        ImVec2 viewportOffset = ImVec2(0, 0);
        ImVec2 viewportSize = ImVec2(0, 0);
        
        ImGui::Begin("Viewport", 0);
        {
            viewportOffset = ImGui::GetCursorPos(); // includes tab bar
            viewportSize = ImGui::GetContentRegionAvail();



            //uint32_t statsWindowWidth = max()

            
            m_ViewportPanelMouseOver = ImGui::IsWindowHovered();
            m_ViewportPanelFocused = ImGui::IsWindowFocused();

            m_ViewportFocused = ImGui::IsWindowFocused();
            m_ViewportHovered = ImGui::IsWindowHovered();

            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

            
    #if _2D
            m_ViewportSize = { viewportSize.x, viewportSize.y };
            ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
    #else

            SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        
            m_EditorScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            if (m_RuntimeScene)
                m_RuntimeScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
            m_EditorCamera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

            ImGui::Image((void*)(intptr_t)SceneRenderer::GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

            static int counter = 0;
            auto windowSize = ImGui::GetWindowSize();
            ImVec2 minBound = ImGui::GetWindowPos();
            minBound.x += viewportOffset.x;
            minBound.y += viewportOffset.y;

            ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
            m_ViewportBounds[0] = { minBound.x, minBound.y };
            m_ViewportBounds[1] = { maxBound.x, maxBound.y };

            m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

    #endif

            // Gizmos
            if (m_GizmoType != -1 && m_SelectionContext.size())
            {
                auto& selection = m_SelectionContext[0];

                float rw = (float)ImGui::GetWindowWidth();
                float rh = (float)ImGui::GetWindowHeight();
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);
                //ImGuizmo::Manipulate(glm::value_ptr(m_Camera.GetViewMatrix()), glm::value_ptr(m_Camera.GetProjectionMatrix()), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(m_Transform));
                //ImGuizmo::Manipulate(
                //    glm::value_ptr(m_ActiveScene->GetCamera().GetViewMatrix()), // view
                //    glm::value_ptr(m_ActiveScene->GetCamera().GetProjectionMatrix()), // projection
                //    (ImGuizmo::OPERATION)m_GizmoType, // operation
                //    ImGuizmo::LOCAL, // mode
                //    glm::value_ptr(m_MeshEntity.Transform())
                //);


                //auto& camera = m_CameraEntity.GetComponent<CameraComponent>().Camera;

                bool snap = Input::GetKey(KeyCode::LeftControl);
                //ImGuizmo::Manipulate(
                //    glm::value_ptr(m_ActiveScene->GetCamera().GetViewMatrix()),// * *m_CurrentlySelectedTransform),
                //    glm::value_ptr(m_ActiveScene->GetCamera().GetProjectionMatrix()),
                //    (ImGuizmo::OPERATION)m_GizmoType,
                //    ImGuizmo::LOCAL,
                //    glm::value_ptr(*m_CurrentlySelectedTransform),
                //    nullptr,
                //    snap ? &m_SnapValue : nullptr
                //);

                auto& entityTransform = selection.Entity.Transform();
                float snapValue[3] = { m_SnapValue, m_SnapValue, m_SnapValue };
                if (m_SelectionMode == SelectionMode::Entity)
                {
                    ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                        glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),

                    //ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()),
                    //    glm::value_ptr(camera.GetProjectionMatrix()),
                        (ImGuizmo::OPERATION)m_GizmoType,
                        ImGuizmo::LOCAL,
                        glm::value_ptr(entityTransform),
                        nullptr,
                        snap ? snapValue : nullptr);
                }
                else
                {
                    glm::mat4 transformBase = entityTransform * selection.Mesh->Transform;


                    ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                        glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),

                    /*ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()),
                        glm::value_ptr(camera.GetProjectionMatrix()),*/
                        (ImGuizmo::OPERATION)m_GizmoType,
                        ImGuizmo::LOCAL,
                        glm::value_ptr(transformBase),
                        nullptr,
                        snap ? snapValue : nullptr);

                    selection.Mesh->Transform = glm::inverse(entityTransform) * transformBase;
                }

            }

            if (showStats)
            {
                static uint32_t statsWindowWidth = 300;
                static uint32_t statsWindowPad = 10;
                ImGui::SetCursorPos(ImVec2(viewportOffset.x + (viewportSize.x - (statsWindowWidth + statsWindowPad * .25f)), viewportOffset.y + statsWindowPad * .25f));
                //ImGui::SetNextWindowSize(ImVec2(128, 128));
                //ImGui::SetNextWindowPos(ImVec2(viewportOffset.x + (viewportSize.x - statsWindowWidth), viewportOffset.y));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, .5f));
                //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
                ImGui::BeginChild("Stats", ImVec2(statsWindowWidth, 225), true
                    ,
                    ImGuiWindowFlags_ChildWindow | 
                    //ImGuiWindowFlags_AlwaysAutoResize | 
                    //ImGuiWindowFlags_NoDocking | 
                    //ImGuiWindowFlags_NoMove | 
                    //ImGuiWindowFlags_NoResize | 
                
                    ImGuiWindowFlags_NoSavedSettings
                );
                ImGui::PopStyleColor();
                //ImGui::PopStyleVar();
                //ImGui::Text("fsdfsdfsf");


                ImVec2 cursorPos = ImGui::GetCursorPos();
                float x = cursorPos.x + statsWindowPad;

                float frameTimeMS = (float)(Time::GetDeltaTime() * 1000.0);

                m_FrameTimeGraph[values_offset] = frameTimeMS;
                values_offset = (values_offset + 1) % 100;

                ImGui::SetCursorPos(ImVec2(x, cursorPos.y + statsWindowPad));
                ImGui::PlotLines("##Frametime", m_FrameTimeGraph, 100, values_offset, "Frametime (ms)", 0.0f, 66.6f, ImVec2(statsWindowWidth - statsWindowPad * 2, 100));
            
                ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
                ImGui::Text("Frametime: %.2fms", frameTimeMS);
            
                ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
                ImGui::Text("FPS: %d", Time::GetFPS());

                auto& caps = RendererAPI::GetCapabilities();

                ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
                ImGui::Text("Vendor: %s", caps.Vendor.c_str());

                ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
                ImGui::Text("Renderer: %s", caps.Renderer.c_str());

                ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
                ImGui::Text("Version: %s", caps.Version.c_str());

                //statsWindowWidth = ImGui::GetWindowSize().x;
                ImGui::EndChild();

            }


            ImGui::End();
        }
        ImGui::PopStyleVar();
        

        m_SceneHierarchyPanel->OnImGuiRender();

        ImGui::Begin("Materials");
        {

            if (m_SelectionContext.size())
            {
                Entity selectedEntity = m_SelectionContext.front().Entity;
                if (selectedEntity.HasComponent<MeshRendererComponent>())
                {
                    MeshRendererComponent* mrComponent = selectedEntity.GetComponent<MeshRendererComponent>();
                    Ref<Mesh> mesh = mrComponent->Mesh;
                    if (mesh)
                    {
                        //auto& materials = mesh->GetMaterialOverrides();
                        //auto& materials = mesh->GetMaterials();

                        auto& materials = mrComponent->Materials;

                        static uint32_t selectedMaterialIndex = 0;
                        for (uint32_t i = 0; i < materials.size(); i++)
                        {
                            auto& materialInstance = materials[i];

                            ImGuiTreeNodeFlags node_flags = (selectedMaterialIndex == i ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;
                            bool opened = ImGui::TreeNodeEx((void*)(&materialInstance), node_flags, materialInstance->GetName().c_str());
                            if (ImGui::IsItemClicked())
                            {
                                selectedMaterialIndex = i;
                            }
                            if (opened)
                                ImGui::TreePop();

                        }

                        ImGui::Separator();

                        if (selectedMaterialIndex < materials.size())
                        {
                            ImGui::Text("Shader: %s", materials[selectedMaterialIndex]->GetShader()->GetName().c_str());
                        }
                    }
                }
            }
            ImGui::End();
        }


        ImGui::End();


        //ToolbarUI();
    }

    void EditorLayer::OnEvent(Ares::Event& e)
    {
        if (m_SceneState == SceneState::Edit)
        {
            if (m_ViewportPanelMouseOver)
                m_EditorCamera.OnEvent(e);

            m_EditorScene->OnEvent(e);
        }
        else if (m_SceneState == SceneState::Play)
        {
            m_RuntimeScene->OnEvent(e);
        }

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ARES_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
        dispatcher.Dispatch<MouseButtonPressedEvent>(ARES_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }
    bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    {

        if (m_ViewportPanelFocused)
        {

            switch (e.GetKeyCode())
            {
            case KeyCode::Q:
                m_GizmoType = -1;
                break;
            case KeyCode::W:
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case KeyCode::E:
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            case KeyCode::R:
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;

            case KeyCode::Delete:
                if (m_SelectionContext.size())
                {
                    Entity selectedEntity = m_SelectionContext[0].Entity;
                    m_EditorScene->DestroyEntity(selectedEntity);
                    m_SelectionContext.clear();
                    m_EditorScene->SetSelectedEntity({});
                    //m_SceneHierarchyPanel->SetSelected({});
                }
                break;
            }
        }
        if (Input::GetKey(KeyCode::LeftControl))
        {
            switch (e.GetKeyCode())
            {



            case KeyCode::G:
                // Toggle grid
                //if (Input::IsKeyPressed(ARES_KEY_LEFT_CONTROL))
                SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
                break;
            case KeyCode::B:
                // Toggle bounding boxes 
                //if (Input::IsKeyPressed(ARES_KEY_LEFT_CONTROL))
            {
                m_UIShowBoundingBoxes = !m_UIShowBoundingBoxes;
                ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
            }
            break;
            case KeyCode::D:
                if (m_SelectionContext.size())
                {
                    Entity selectedEntity = m_SelectionContext[0].Entity;
                    m_EditorScene->DuplicateEntity(selectedEntity);
                }
                break;
            }
        }
        return false;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        auto [mx, my] = Input::GetMousePosition();
        if (e.GetMouseButton() == MouseButtonCode::ButtonLeft && !Input::GetKey(KeyCode::LeftAlt) && !ImGuizmo::IsOver() && m_SceneState != SceneState::Play)
        {
            auto [mouseX, mouseY] = GetMouseViewportSpace();
            if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
            {
                auto [origin, direction] = CastRay(mouseX, mouseY);

                //m_SelectedSubmeshes.clear();

                //m_ActiveScene->m_Registry.each([&](auto entity)

                m_SelectionContext.clear();
                //auto meshEntities = m_ActiveScene->GetAllEntitiesWith<MeshRendererComponent>();
                m_EditorScene->SetSelectedEntity({});
                auto meshEntities = m_EditorScene->GetAllEntitiesWith<MeshRendererComponent>();


                for (auto e : meshEntities)

                {
                    Entity entity = { e, m_EditorScene.get() };
                    auto mesh = entity.GetComponent<MeshRendererComponent>()->Mesh;
                    if (!mesh)
                        continue;

                    
                    //Entity entityS = m_ActiveScene->EntityConstructor(entity);

                    /*if (!entityS.HasComponent<MeshRendererComponent>())
                        return;
                    
                    MeshRendererComponent& mrComponent = entityS.GetComponent<MeshRendererComponent>();
                    if (!mrComponent.Mesh)
                        return;*/
                    //auto mesh = entityS.GetComponent<MeshRendererComponent>().Mesh;

                    auto& submeshes = mesh->GetSubmeshes();
                    float lastT = std::numeric_limits<float>::max();
                    for (uint32_t i = 0; i < submeshes.size(); i++)
                    {
                        auto& submesh = submeshes[i];
                        Ray ray = {
                            glm::inverse(
                                entity.GetComponent<TransformComponent>()->Transform * submesh.Transform
                            ) * glm::vec4(origin, 1.0f),
                            glm::inverse(glm::mat3(entity.GetComponent<TransformComponent>()->Transform) * glm::mat3(submesh.Transform)) * direction
                        };

                        float t;
                        bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
                        if (intersects)
                        {
                            const auto& triangleCache = mesh->GetTriangleCache(i);
                            for (const auto& triangle : triangleCache)
                            {
                                if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
                                {
                                    ARES_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
                                    //m_SelectedSubmeshes.push_back({ entityS, &submesh, t });
                                    m_SelectionContext.push_back({ entity, &submesh, t });

                                    break;
                                }
                            }
                        }
                    }
                }//);

                std::sort(m_SelectionContext.begin(), m_SelectionContext.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
                if (m_SelectionContext.size())
                    OnSelected(m_SelectionContext[0]);

                //std::sort(m_SelectedSubmeshes.begin(), m_SelectedSubmeshes.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });

                //// TODO: Handle mesh being deleted, etc.
                //if (m_SelectedSubmeshes.size())
                //    m_CurrentlySelectedTransform = &m_SelectedSubmeshes[0].Mesh->Transform;
                //else
                //    m_CurrentlySelectedTransform = nullptr;// &m_MeshEntity.GetComponent<TransformComponent>().Transform;

            }
        }
        return false;
    }

    std::pair<float, float> EditorLayer::GetMouseViewportSpace()
    {
        auto [mx, my] = ImGui::GetMousePos(); // Input::GetMousePosition();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
        auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

        return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
    }

    std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float mx, float my)
    {
        glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };

        auto inverseProj = glm::inverse(m_EditorCamera.GetProjectionMatrix());
        auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.GetViewMatrix()));

        /*auto inverseProj = glm::inverse(m_CameraEntity.GetComponent<CameraComponent>().Camera.GetProjectionMatrix());
        auto inverseView = glm::inverse(glm::mat3(m_CameraEntity.GetComponent<CameraComponent>().Camera.GetViewMatrix()));*/

        /*auto inverseProj = glm::inverse(m_Scene->GetCamera().GetProjectionMatrix());
        auto inverseView = glm::inverse(glm::mat3(m_Scene->GetCamera().GetViewMatrix()));*/

        glm::vec4 ray = inverseProj * mouseClipPos;

        //glm::vec3 rayPos = m_Scene->GetCamera().GetPosition();
        //glm::vec3 rayPos = m_CameraEntity.GetComponent<CameraComponent>().Camera.GetPosition();
        glm::vec3 rayPos = m_EditorCamera.GetPosition();

        glm::vec3 rayDir = inverseView * glm::vec3(ray);

        return { rayPos, rayDir };
    }

    void EditorLayer::OnSelected(const SelectedSubmesh& selectionContext)
    {
        m_SceneHierarchyPanel->SetSelected(selectionContext.Entity);
        m_EditorScene->SetSelectedEntity(selectionContext.Entity);
    }

    void EditorLayer::OnEntityDeleted(Entity e)
    {
        if (m_SelectionContext[0].Entity == e)
        {
            m_SelectionContext.clear();
            m_EditorScene->SetSelectedEntity({});
        }
    }


    Ray EditorLayer::CastMouseRay()
    {
        auto [mouseX, mouseY] = GetMouseViewportSpace();
        if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
        {
            auto [origin, direction] = CastRay(mouseX, mouseY);
            return Ray(origin, direction);
        }
        return Ray::Zero();
    }


}

