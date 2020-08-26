#include "PhobosPCH.h"
#include "EditorLayer.h"
#include "EditorGUI.h"
#include "Ares/ImGui/ImGuizmo.h"
#include <filesystem>
#include "MaterialEditor.h"
#include "EditorUtility.h"
#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
#define _2D 0


/*
    save open save as scene key shortcuts
*/
namespace Ares
{

    EditorLayer::EditorLayer()
        : Layer("Phobos Editor"), 
        m_CameraController(1280.0f / 720.0f),
        m_EditorCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
    {
    }

    void InitializeGUIColors()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Window background
        colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f); // Widget backgrounds
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
        colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
        colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);

        colors[ImGuiCol_Tab] = ImColor(100, 100, 100, 255);

    }

    void EditorLayer::OnAttach()
    {
        m_FileSystemWatcher.Watch();
        InitializeGUIColors();

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
        m_AssetManagerPanel = CreateScope<AssetManagerPanel>();


        Ref<TextureCube> skyCube = SceneRenderer::ConvertHDRToCubemap(
            "C:\\Users\\Andres\\Desktop\\DevProjects\\Hazel\\Hazel-dev\\Hazelnut\\assets\\env\\venice_dawn_1_4k.hdr"
        );
        
        //auto environment = Environment::Load(
            //"C:\\Users\\Andres\\Desktop\\DevProjects\\Hazel\\Hazel-dev\\Hazelnut\\assets\\env\\venice_dawn_1_4k.hdr"
        //);

        m_EditorScene = CreateRef<Scene>("Editor Scene");
        UpdateWindowTitle("Editor Scene");

        //auto skyboxShader = Shader::Find("Assets/Shaders/CubemapSkybox.glsl");
        auto skyboxMaterial = CreateRef<Material>(Shader::Find("Assets/Shaders/CubemapSkybox.glsl"));
        skyboxMaterial->SetTexture("u_Texture", skyCube);
        skyboxMaterial->SetFlag(MaterialFlag::DepthTest, true);


        m_EditorScene->SetSkyboxMaterial(skyboxMaterial);
        //m_EditorScene->SetEnvironment(environment);

        std::vector<Ref<Material>> loadedMaterials;
        std::vector<Ref<Animation>> loadedAnimations;
        Ref<Mesh> loadedMesh;
        Ref<ModelNodeMap> loadedModelNodeMap;

        ModelLoading::LoadModel(
            "C:\\Users\\Andres\\Desktop\\DevProjects\\Hazel\\Hazel-dev\\Hazelnut\\assets\\models\\m1911\\M1911Materials.fbx", 
            loadedMesh, 
            loadedModelNodeMap,
            loadedMaterials,
            loadedAnimations
        );
        
        if (!loadedMaterials.size())
        {
            Ref<Material> m_MeshBaseMaterial = CreateRef<Material>(Shader::Find("Assets/Shaders/Standard.glsl"));
            ARES_WARN("Couldnt Find Materials... setting custom");
            loadedMaterials = { m_MeshBaseMaterial };
        }

        {
            Entity gunEntity = m_EditorScene->CreateEntity("Gun");
            MeshRendererComponent* mr = gunEntity.AddComponent<MeshRendererComponent>();

            mr->Mesh = loadedMesh;
            mr->Materials = loadedMaterials;
            
            AnimatorComponent* animator = gunEntity.AddComponent<AnimatorComponent>();
            animator->Animator.SetAnimationNodeInformation(loadedModelNodeMap);
            animator->Animator.SetAnimations(loadedAnimations);

            gunEntity.GetComponent<TransformComponent>()->SetWorldTransform( glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 2)),
                glm::vec3(15.0f)
            ));

        }

        {
            Entity gunEntity = m_EditorScene->CreateEntity("Gun");
            MeshRendererComponent* mr = gunEntity.AddComponent<MeshRendererComponent>();

            mr->Mesh = loadedMesh;
            mr->Materials = loadedMaterials;
            
            AnimatorComponent* animator = gunEntity.AddComponent<AnimatorComponent>();
            animator->Animator.SetAnimationNodeInformation(loadedModelNodeMap);
            animator->Animator.SetAnimations(loadedAnimations);

            gunEntity.GetComponent<TransformComponent>()->SetWorldTransform(glm::scale(
                glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2)),
                glm::vec3(15.0f)
            ));

        }


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

                sphereEntity.GetComponent<TransformComponent>()->SetWorldTransform( glm::translate(
                    glm::mat4(1.0f), 
                    glm::vec3(x * spread, y * spread, 0)
                ));
            }
        }

        auto& light = m_EditorScene->GetLight();
        light.Direction = { -0.5f, -0.5f, 0.5f };
        light.Radiance = { 1.0f, 1.0f, 1.0f };

        // SceneSerializer serializer(m_EditorScene);
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
        m_SelectedEntity = {};

        m_SceneState = SceneState::Play;

        m_RuntimeScene = CreateRef<Scene>("Runtime Scene");
        m_EditorScene->CopyTo(m_RuntimeScene);

        m_RuntimeScene->OnRuntimeStart();
    }
    void EditorLayer::OnSceneStop()
    {
        m_RuntimeScene->OnRuntimeStop();
        m_SceneState = SceneState::Edit;

        // Unload runtime scene
        m_RuntimeScene = nullptr;

        m_SelectedEntity = {};
    }

    void EditorLayer::UpdateWindowTitle(const std::string& sceneName)
    {
        std::string title = sceneName + " - Phobos - " + Application::GetPlatformName() + " (" + Application::GetConfigurationName() + ")";
        Application::Get().GetWindow().SetTitle(title);
    }

    void EditorLayer::OnUpdate()
    {        
        
        // undo redo
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

        if (m_ViewportPanelFocused)
        {
            if (Input::GetKeyDown(KeyCode::D1))
            {
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            }
            if (Input::GetKeyDown(KeyCode::D2))
            {
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            }
            if (Input::GetKeyDown(KeyCode::D3))
            {
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
            }
            if (Input::GetKeyDown(KeyCode::D4))
            {
                m_EditorCamera.m_FreeCamMode = !m_EditorCamera.m_FreeCamMode;
            }

            if (m_SelectedEntity)
            {
                if (Input::GetKeyDown(KeyCode::F))
                {
                    m_EditorCamera.Focus(m_SelectedEntity.GetComponent<TransformComponent>()->GetWorldTransform()[3], 5);
                }
            }

            if (Input::GetKeyDown(KeyCode::Space))
            {
                m_EditorCamera.StraightenAngles();
            }
        }

        if (m_ViewportPanelFocused || m_SceneHierarchyFocused)
        {
            if (m_SelectedEntity)
            {
                if (Input::GetKey(KeyCode::LeftControl) || Input::GetKey(KeyCode::RightControl))
                {
                    if (Input::GetKeyDown(KeyCode::D))
                    {
                        // duplicate
                        m_EditorScene->DuplicateEntity(m_SelectedEntity);
                        // TODO: undo/redo duplicating
                    }
                    if (Input::GetKeyDown(KeyCode::Delete) || Input::GetKeyDown(KeyCode::Backspace))
                    {
                        // delete
                        m_EditorScene->DestroyEntity(m_SelectedEntity);
                        m_SelectedEntity = {};
                        // TODO: undo/redo deleting
                    }
                }
            }
        }



        switch (m_SceneState)
        {
        case SceneState::Edit:
        {
            if (m_ViewportPanelFocused)
                m_EditorCamera.Update();

            m_EditorScene->OnRenderEditor(m_EditorCamera, m_EditorCamera.m_ViewMatrix, m_SelectedEntity);
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
    

#endif


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

        ImVec2 buttonSize = ImVec2(37, 37);

        ImVec4 backGroundActive = ImGui::GetStyle().Colors[ImGuiCol_TabActive];
        ImVec4 activeTint = ImGui::GetStyle().Colors[ImGuiCol_Text];

        float buttonPad = 10;

        float xl = buttonPad;
        ImGui::SetCursorPos(ImVec2(xl, ImGui::GetCursorPos().y));

        if (EditorGUI::EditorImageButton(EditorResources::GetTexture("Translate.png"), buttonSize, m_GizmoType == ImGuizmo::OPERATION::TRANSLATE))
        {
            m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
        }

        xl += buttonSize.x + buttonPad;
        ImGui::SameLine(xl);
        if (EditorGUI::EditorImageButton(EditorResources::GetTexture("Rotate.png"), buttonSize, m_GizmoType == ImGuizmo::OPERATION::ROTATE))
        {
            m_GizmoType = ImGuizmo::OPERATION::ROTATE;
        }

        xl += buttonSize.x + buttonPad;
        ImGui::SameLine(xl);
        if (EditorGUI::EditorImageButton(EditorResources::GetTexture("Scale.png"), buttonSize, m_GizmoType == ImGuizmo::OPERATION::SCALE))
        {
            m_GizmoType = ImGuizmo::OPERATION::SCALE;    
        }








        
        xl += buttonSize.x + buttonPad;
        ImGui::SameLine(xl);

        float divSize = 10;
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyle().Colors[ImGuiCol_Button]);
        ImGui::Button("", ImVec2(divSize, toolbarSize));
        ImGui::PopStyleColor(3);

        float buttonW = 100;
        xl += divSize + buttonPad;
        ImGui::SameLine(xl);

        const char* camName = m_EditorCamera.m_FreeCamMode ? "Free Cam" : "Editor Cam";
        if (EditorGUI::EditorButton(camName, buttonW, false))
        {
            m_EditorCamera.m_FreeCamMode = !m_EditorCamera.m_FreeCamMode;
        }

        float mx = toolbarWidth * .5f - buttonSize.x * .5f;
        ImGui::SameLine(mx);
        if (EditorGUI::EditorImageButton(EditorResources::GetTexture("play.png"), buttonSize, m_SceneState == SceneState::Play))
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



        float buttonWidth = 100;
        float xr = (toolbarWidth - buttonPad) - buttonWidth;
        ImGui::SameLine(xr);
        if (EditorGUI::EditorButton("Show Stats", buttonWidth, showStats))
        {
            showStats = !showStats;
        }

        xr = (xr - buttonPad) - buttonWidth;
        ImGui::SameLine(xr);

        if (EditorGUI::EditorButton("Show AABB", buttonWidth, SceneRenderer::GetOptions().ShowBoundingBoxes))
        {
            SceneRenderer::GetOptions().ShowBoundingBoxes = !SceneRenderer::GetOptions().ShowBoundingBoxes;
        }
        
        xr = (xr - buttonPad) - buttonWidth;
        ImGui::SameLine(xr);
        if (EditorGUI::EditorButton("Show Grid", buttonWidth, SceneRenderer::GetOptions().ShowGrid))
        {
            SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
        }

        
        
        ImGui::End();
    }

    static bool dockspaceOpen = true;



    void EditorLayer::OpenScene()
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
            m_SelectedEntity = {};
            m_SceneFilePath = filepath;
        }

    }

    void EditorLayer::SaveScene()
    {
        SceneSerializer serializer(m_EditorScene);
        serializer.Serialize(m_SceneFilePath);
    }

    void EditorLayer::SaveSceneAs()
    {
        auto& app = Application::Get();
        std::string filepath = app.SaveFile("Hazel Scene (*.hsc)\0*.hsc\0");
        if (!filepath.empty())
        {
            SceneSerializer serializer(m_EditorScene);
            serializer.Serialize(filepath);

            std::filesystem::path path = filepath;
            UpdateWindowTitle(path.filename().string());
            m_SceneFilePath = filepath;
        }
    }



    // to do: add edit
    void EditorLayer::DrawMenu()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {

                if (ImGui::MenuItem("New Scene", "Ctrl/Cmd + N"))
                {

                }
                if (ImGui::MenuItem("Open Scene", "Ctrl/Cmd + O"))
                    OpenScene();

                ImGui::Separator();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                    dockspaceOpen = false;

                    Ares::Application::Get().Close();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl/Cmd + Z", false, EditorUtility::CanUndo()))
                {
                    EditorUtility::TriggerUndo();
                }
                if (ImGui::MenuItem("Redo", "Ctrl/Cmd + Shift + Z", false, EditorUtility::CanRedo()))
                {
                    EditorUtility::TriggerRedo();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }


    float EditorLayer::GetSnapValue()
    {
        switch (m_GizmoType)
        {
        case  ImGuizmo::OPERATION::TRANSLATE: return 0.5f;
        case  ImGuizmo::OPERATION::ROTATE: return 45.0f;
        case  ImGuizmo::OPERATION::SCALE: return 0.5f;
        }
        return 0.0f;
    }

    void EditorLayer::DrawSceneViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::Begin("Viewport", 0);
        {
            ImVec2 viewportOffset = ImGui::GetCursorPos(); // includes tab bar
            ImVec2 viewportSize = ImGui::GetContentRegionAvail();

            m_ViewportPanelMouseOver = ImGui::IsWindowHovered();
            m_ViewportPanelFocused = ImGui::IsWindowFocused();

            Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportPanelFocused || !m_ViewportPanelMouseOver);

#if _2D
            m_ViewportSize = { viewportSize.x, viewportSize.y };
            ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
#else

            SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

            m_EditorScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            if (m_RuntimeScene)
                m_RuntimeScene->OnViewportResize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            
            m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
            
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
            float rw = (float)ImGui::GetWindowWidth();
            float rh = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

            glm::mat4 im = glm::mat4(1.0);
            
            if (!m_EditorCamera.m_FreeCamMode)
            {
                float viewManipSize = 128;
                ImGuizmo::ViewManipulate(
                    glm::value_ptr(m_EditorCamera.m_ViewMatrix), m_EditorCamera.m_Distance, 
                    ImVec2(ImGui::GetWindowPos().x + (ImGui::GetWindowWidth() - viewManipSize), ImGui::GetWindowPos().y + (ImGui::GetWindowHeight() - viewManipSize)), 
                    ImVec2(viewManipSize, viewManipSize), 
                    0x80808080
                );
            }
            
            // Gizmos
            if (m_GizmoType != -1 && m_SelectedEntity)
            {
                bool snap = Input::GetKey(KeyCode::LeftControl);
                
                auto& entityTransform = m_SelectedEntity.GetComponent<TransformComponent>()->GetWorldTransform();
                float snapValue = GetSnapValue();
                float snapValues[3] = { snapValue, snapValue, snapValue };

                ImGuizmo::Manipulate(
                    glm::value_ptr(m_EditorCamera.m_ViewMatrix),
                    glm::value_ptr(m_EditorCamera.m_ProjectionMatrix),
                    (ImGuizmo::OPERATION)m_GizmoType,
                    ImGuizmo::LOCAL,
                    glm::value_ptr(entityTransform),
                    nullptr,
                    snap ? snapValues : nullptr
                );

                m_SelectedEntity.GetComponent<TransformComponent>()->SetWorldTransform(entityTransform);
            }

            if (showStats)
            {
                m_StatsWindow.DrawWindow(viewportOffset, viewportSize, 300, 225, 10);
            }


            ImGui::End();
        }

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::PopStyleVar();
    }

    // camera straighten angles and focus



    const std::string s_ImGuiColorKeys[]{
        "Text",
        "TextDisabled",
        "WindowBg",              // Background of normal windows
        "ChildBg",               // Background of child windows
        "PopupBg",               // Background of popups, menus, tooltips windows
        "Border",
        "BorderShadow",
        "FrameBg",               // Background of checkbox, radio button, plot, slider, text input
        "FrameBgHovered",
        "FrameBgActive",
        "TitleBg",
        "TitleBgActive",
        "TitleBgCollapsed",
        "MenuBarBg",
        "ScrollbarBg",
        "ScrollbarGrab",
        "ScrollbarGrabHovered",
        "ScrollbarGrabActive",
        "CheckMark",
        "SliderGrab",
        "SliderGrabActive",
        "Button",
        "ButtonHovered",
        "ButtonActive",
        "Header",                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
        "HeaderHovered",
        "HeaderActive",
        "Separator",
        "SeparatorHovered",
        "SeparatorActive",
        "ResizeGrip",
        "ResizeGripHovered",
        "ResizeGripActive",
        "Tab",
        "TabHovered",
        "TabActive",
        "TabUnfocused",
        "TabUnfocusedActive",
        "DockingPreview",        // Preview overlay color when about to docking something
        "DockingEmptyBg",        // Background color for empty node (e.g. CentralNode with no window docked into it)
        "PlotLines",
        "PlotLinesHovered",
        "PlotHistogram",
        "PlotHistogramHovered",
        "TextSelectedBg",
        "DragDropTarget",
        "NavHighlight",          // Gamepad/keyboard: current highlighted item
        "NavWindowingHighlight", // Highlight window when using CTRL+TAB
        "NavWindowingDimBg",     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
        "ModalWindowDimBg",
    };


    void EditorLayer::DrawEditorPreferencesWindow()
    {
        ImGui::Begin("Editor Settings:");

        if (ImGui::CollapsingHeader("Scene View", nullptr, ImGuiTreeNodeFlags_Selected))
        {
            EditorGUI::Color4Field("AABB Color", SceneRenderer::GetOptions().AABBColor);
            EditorGUI::Color4Field("Triangles Color", SceneRenderer::GetOptions().MeshTrianglesColor);

        }
        ImGui::Separator();
        static bool open0 = true;
        if (ImGui::CollapsingHeader("Scene View Grid", nullptr, ImGuiTreeNodeFlags_Selected))
        {
            EditorGUI::Color4Field("Color", SceneRenderer::GetOptions().GridColor);
            EditorGUI::IntSliderField("Resolution", SceneRenderer::GetOptions().GridResolution, 1, 100);
            EditorGUI::Vec2Field("Camera Range", SceneRenderer::GetOptions().GridCameraRange);
        }

        ImGui::Separator();
        static bool open1 = true;
        if (ImGui::CollapsingHeader("Scene View Free Camera Speeds", nullptr, ImGuiTreeNodeFlags_Selected))
        {

            ImGui::Columns(2);
            EditorGUI::FloatSliderField("Move Speed Fast", m_EditorCamera.m_MoveSpeedFast, 1, 50);
            EditorGUI::FloatSliderField("Move Speed", m_EditorCamera.m_MoveSpeed, 1, 50);
            EditorGUI::FloatSliderField("Rotation Speed Fast", m_EditorCamera.m_RotSpeedFast, 1, 50);
            EditorGUI::FloatSliderField("Rotation Speed", m_EditorCamera.m_RotSpeed, 1, 50);
            ImGui::Columns(1);
        }
        ImGui::Separator();
        static bool open2 = true;
        if (ImGui::CollapsingHeader("Scene View Camera Speeds", nullptr, ImGuiTreeNodeFlags_Selected))
        {

            ImGui::Columns(2);
            EditorGUI::Vec2SliderField("Pan Speed", m_EditorCamera.m_PanSpeed, .1f, 50);
            EditorGUI::FloatSliderField("Orbit Speed", m_EditorCamera.m_OrbitSpeed, .1, 50);
            
            ImGui::Columns(1);
        }
        ImGui::Separator();

        static bool open3 = true;
        if (ImGui::CollapsingHeader("Editor Colors", nullptr, ImGuiTreeNodeFlags_Selected))
        {
            ImGui::Columns(2);
            ImVec4* colors = ImGui::GetStyle().Colors;

            for (uint32_t i = 0; i < ImGuiCol_COUNT; i++)
            {
                EditorGUI::Color4Field(s_ImGuiColorKeys[i], colors[i]);
            }

            ImGui::Columns(1);
        }

        ImGui::End();
    } 





        
    void EditorLayer::OnImGuiDraw()
    {

        ARES_PROFILE_FUNCTION();

        // ImGui::ShowDemoWindow();

        ToolbarUI();

        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImVec2 wPos = viewport->GetWorkPos();
        ImVec2 wSize = viewport->GetWorkSize();
        ImGui::SetNextWindowPos(ImVec2(wPos.x, wPos.y + (toolbarSize + menuBarHeight)));
        ImGui::SetNextWindowSize(ImVec2(wSize.x, wSize.y - (toolbarSize + menuBarHeight)));

        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        window_flags |= ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus;


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

        ImGui::PopStyleVar(3);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        DrawEditorPreferencesWindow();

        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        DrawSceneViewport();
        
        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        m_AssetManagerPanel->RenderAssetWindow();

        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        Entity doubleClickedEntity;
        SceneHierarchyPanel::Draw(m_EditorScene, m_SelectedEntity, doubleClickedEntity, m_SceneHierarchyFocused);// , deletedEntity);
        if (doubleClickedEntity)
            m_EditorCamera.Focus(doubleClickedEntity.GetComponent<TransformComponent>()->GetWorldTransform()[3], 5);
        
        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        m_Console.Render();

        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);
        InspectorPanel::DrawInspectorForEntity(m_SelectedEntity);

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
        ImGui::Begin("Scene Settings");
        {
                /*
            if (ImGui::Button("Load Environment Map"))
            {
                std::string filename = Application::Get().OpenFile("*.hdr");
                if (filename != "")
                    m_EditorScene->SetEnvironment(Environment::Load(filename));
            }
                */
            ImGui::Columns(2);
            ImGui::AlignTextToFramePadding();
            EditorGUI::FloatSliderField("Exposure", m_EditorScene->GetExposure(), 0.0f, 5.0f);

            auto& light = m_EditorScene->GetLight();
            EditorGUI::Vec3Field("Light Direction", light.Direction);
            EditorGUI::Color3Field("Light Radiance", light.Radiance);
            EditorGUI::FloatSliderField("Light Multiplier", light.Multiplier, 0.0f, 5.0f);
            ImGui::Columns(1);

            if (ImGui::TreeNode("Shaders"))
            {
                auto& shaders = Shader::s_AllShaders;
                for (auto& shader : shaders)
                {
                    if (ImGui::TreeNode(shader->GetName().c_str()))
                    {
                        std::string buttonName = "Open##" + shader->GetName();
                        if (ImGui::Button(buttonName.c_str()))
                        {
                            // TODO: have user specify code editor of choice
                            int r = system(("code " + shader->GetPath()).c_str());

                            if (r)
                            {
                                ARES_CORE_ERROR("Problem With Shader Open");
                            }

                        }

                        buttonName = "Reload##" + shader->GetName();
                        if (ImGui::Button(buttonName.c_str()))
                            shader->Reload();
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
            ImGui::End();
        }

        

#endif
        
        ImGui::End();
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
        dispatcher.Dispatch<MouseButtonPressedEvent>(ARES_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == MouseButtonCode::ButtonLeft && !Input::GetKey(KeyCode::LeftAlt) && !ImGuizmo::IsOver() && m_SceneState != SceneState::Play)
        {
            auto [mouseX, mouseY] = GetMouseViewportSpace();
            if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
            {
                auto [origin, direction] = CastRay(mouseX, mouseY);

                std::vector<SelectedSubmesh> m_SelectionContext;
                m_SelectedEntity = {};

                auto meshEntities = m_EditorScene->GetAllEntitiesWith<MeshRendererComponent>();

                for (auto e : meshEntities)

                {
                    Entity entity = { e, m_EditorScene.get() };
                    auto mesh = entity.GetComponent<MeshRendererComponent>()->Mesh;
                    if (!mesh)
                        continue;

                    auto& submeshes = mesh->GetSubmeshes();
                    for (uint32_t i = 0; i < submeshes.size(); i++)
                    {
                        auto& submesh = submeshes[i];
                        Ray ray = {
                            glm::inverse(
                                entity.GetComponent<TransformComponent>()->GetWorldTransform() * submesh.ModelNode->GetModelSpaceTransform()
                            ) * glm::vec4(origin, 1.0f),
                            glm::inverse(glm::mat3(entity.GetComponent<TransformComponent>()->GetWorldTransform()) * glm::mat3(submesh.ModelNode->GetModelSpaceTransform())) * direction
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
                                    m_SelectionContext.push_back({ entity, t });
                                    break;
                                }
                            }
                        }
                    }
                }

                std::sort(m_SelectionContext.begin(), m_SelectionContext.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
                if (m_SelectionContext.size())
                {
                    m_SelectedEntity = m_SelectionContext[0].Entity;
                }
            }
        }
        return false;
    }

    std::pair<float, float> EditorLayer::GetMouseViewportSpace()
    {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        auto viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
        auto viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;
        return { (mx / viewportWidth) * 2.0f - 1.0f, ((my / viewportHeight) * 2.0f - 1.0f) * -1.0f };
    }

    std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float mx, float my)
    {
        glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };
        auto inverseProj = glm::inverse(m_EditorCamera.m_ProjectionMatrix);
        auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.m_ViewMatrix));
        glm::vec4 ray = inverseProj * mouseClipPos;
        glm::vec3 rayPos = glm::inverse(m_EditorCamera.m_ViewMatrix)[3];
        glm::vec3 rayDir = inverseView * glm::vec3(ray);
        return { rayPos, rayDir };
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
