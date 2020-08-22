#include "PhobosPCH.h"
#include "EditorLayer.h"
#include "EditorGUI.h"
#include "Ares/ImGui/ImGuizmo.h"
#include <filesystem>
#include "MaterialEditor.h"
#include "EditorUtility.h"
#include "SceneHierarchyPanel.h"

#define _2D 0

/*
    TODO:
    display grid as lines2D

*/
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
        //memset(m_FrameTimeGraph, 0, sizeof(float) * 100);

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
        //m_PlayButtonTex = EditorResources::GetTexture("play.png");

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

        //m_SceneHierarchyPanel = CreateScope<SceneHierarchyPanel>(m_EditorScene);
        //m_SceneHierarchyPanel->SetSelectionChangedCallback(std::bind(&EditorLayer::SelectEntity, this, std::placeholders::_1));
        //m_SceneHierarchyPanel->SetEntityDeletedCallback(std::bind(&EditorLayer::OnEntityDeleted, this, std::placeholders::_1));
        

        

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
        m_SelectedEntity = {};
        //m_SelectionContext.clear();


        m_SceneState = SceneState::Play;

        m_RuntimeScene = CreateRef<Scene>("Runtime Scene");
        m_EditorScene->CopyTo(m_RuntimeScene);

        m_RuntimeScene->OnRuntimeStart();
        //m_SceneHierarchyPanel->SetContext(m_RuntimeScene);
    }
    void EditorLayer::OnSceneStop()
    {
        m_RuntimeScene->OnRuntimeStop();
        m_SceneState = SceneState::Edit;

        // Unload runtime scene
        m_RuntimeScene = nullptr;

        m_SelectedEntity = {};
        //m_SelectionContext.clear();

        //m_SceneHierarchyPanel->SetContext(m_EditorScene);
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
                //m_EditorCamera.m_MoveMode = (EditorCamera::MoveMode)((((uint32_t)m_EditorCamera.m_MoveMode) + 1) % 3);
            }

            if (m_SelectedEntity)
            {
                if (Input::GetKeyDown(KeyCode::F))
                {
                    m_EditorCamera.Focus(m_SelectedEntity.GetComponent<TransformComponent>()->Transform[3], 5);
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
            //if (m_ViewportPanelFocused)
                m_EditorCamera.Update();

            m_EditorScene->OnRenderEditor(m_EditorCamera, m_EditorCamera.m_ViewMatrix, m_SelectedEntity);

            /*
            if (m_DrawOnTopBoundingBoxes)
            {
                Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
                auto viewProj = m_EditorCamera.GetViewProjection();
                Renderer2D::BeginScene(viewProj, false);
                // TODO: Renderer::DrawAABB(m_MeshEntity.GetComponent<MeshComponent>(), m_MeshEntity.GetComponent<TransformComponent>());
                Renderer2D::EndScene();
                Renderer::EndRenderPass();
            }
            */


            //Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false, false, false);
            //Renderer::BeginRenderPass(SceneRenderer::GetGeometryPass(), false, false, false);

            /*
            auto viewProj = m_EditorCamera.GetViewProjection();
            Renderer2D::BeginScene(viewProj, glm::inverse(m_EditorCamera.m_ViewMatrix)[3]);// , false);
            
            //if (m_SelectionContext.size() && false)
            if (m_SelectedEntity)
            {

                //auto& selection = m_SelectionContext[0];
                //if (selection.Mesh && selection.Entity.HasComponent<MeshRendererComponent>())
                if (m_SelectedEntity.HasComponent<MeshRendererComponent>())
                {
                    MeshRendererComponent* mr = m_SelectedEntity.GetComponent<MeshRendererComponent>();
                    //glm::vec4 color = (m_SelectionMode == SelectionMode::Entity) ? glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f } : glm::vec4{ 0.2f, 0.9f, 0.2f, 1.0f };
                    glm::vec4 color = glm::vec4{ 0.2f, 0.9f, 0.2f, 1.0f };
                    
                    Renderer::DrawAABB(mr->Mesh, m_SelectedEntity.GetComponent<TransformComponent>()->Transform, color, true);

                    //Renderer::DrawAABB(mr->Mesh->GetBounds(), selection.Entity.GetComponent<TransformComponent>()->Transform * selection.Mesh->Transform, color);
                    //Renderer::DrawAABB(selection.Mesh->BoundingBox, selection.Entity.GetComponent<TransformComponent>()->Transform * selection.Mesh->Transform, color);



                    

                }
            }


            
            Vector3 camPos = glm::inverse(m_EditorCamera.m_ViewMatrix)[3];
            camPos.y = 0;
            camPos.x = (int)camPos.x;
            camPos.z = (int)camPos.z;



            Vector4 gridColor = { .8f, .8f, .8f, .5f };
            const int gridRes = 30;
            for (int i = -m_GridResolution; i <= m_GridResolution; i++)
            {

                Renderer2D::SubmitLine(camPos + Vector3{ i, 0, -m_GridResolution }, camPos + Vector3{ i, 0, m_GridResolution }, m_GridColor, true, m_GridCameraRange);
                //Renderer2D::SubmitLine(camPos + Vector3{ -m_GridResolution, 0, i }, camPos + Vector3{ m_GridResolution, 0, i }, m_GridColor, true, m_GridCameraRange);
            }



            Renderer2D::EndScene();
            Renderer::EndRenderPass();
            */
            
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
    /*
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
    */


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

        //if (EditorGUI::EditorButton("Cam Default", buttonW, m_EditorCamera.m_MoveMode == EditorCamera::MoveMode::Default))
        if (EditorGUI::EditorButton(camName, buttonW, false))
        {
            m_EditorCamera.m_FreeCamMode = !m_EditorCamera.m_FreeCamMode;
            //m_EditorCamera.m_MoveMode = EditorCamera::MoveMode::Default;
        }

        /*
        xl += buttonW + buttonPad;
        ImGui::SameLine(xl);
        if (EditorGUI::EditorButton("Cam Pan", buttonW, m_EditorCamera.m_MoveMode == EditorCamera::MoveMode::Pan))
        {
            m_EditorCamera.m_MoveMode = EditorCamera::MoveMode::Pan;
        }
        xl += buttonW + buttonPad;
        ImGui::SameLine(xl);
        if (EditorGUI::EditorButton("Free Cam", buttonW, m_EditorCamera.m_MoveMode == EditorCamera::MoveMode::FreeCamera))
        {
            m_EditorCamera.m_MoveMode = EditorCamera::MoveMode::FreeCamera;
        }
        */




        




        
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
        /*
        */

        xr = (xr - buttonPad) - buttonWidth;
        ImGui::SameLine(xr);
        if (EditorGUI::EditorButton("Show Grid", buttonWidth, SceneRenderer::GetOptions().ShowGrid))
        {
            SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
        }

        
        
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

                if (ImGui::MenuItem("New Scene", "Ctrl/Cmd + N"))
                {

                }
                if (ImGui::MenuItem("Open Scene"))
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
                        //m_SceneHierarchyPanel->SetContext(m_EditorScene);

                        m_SelectedEntity = {};
                        //m_EditorScene->SetSelectedEntity({});
                        //m_SelectionContext.clear();
                    }
                }
                if (ImGui::MenuItem("Save Scene", "Ctrl/Cmd + S"))
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


    void EditorLayer::DrawSceneViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("Viewport", 0);//, ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus);
        {
            ImVec2 viewportOffset = ImGui::GetCursorPos(); // includes tab bar
            ImVec2 viewportSize = ImGui::GetContentRegionAvail();

            m_ViewportPanelMouseOver = ImGui::IsWindowHovered();
            m_ViewportPanelFocused = ImGui::IsWindowFocused();

            //m_ViewportFocused = ImGui::IsWindowFocused();
            //m_ViewportHovered = ImGui::IsWindowHovered();

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
            //m_EditorCamera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

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
            /*
            ImGuizmo::DrawGrid(
                glm::value_ptr(m_EditorCamera.m_ViewMatrix),
                glm::value_ptr(m_EditorCamera.m_ProjectionMatrix),
                glm::value_ptr(im), 
                10.0f);
            */


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
            //m_EditorCamera.SetViewMatrix(m_EditorCamera.m_ViewMatrix);

            //ImGuizmo::SetOrthographic(false);

            //ImGuizmo::ViewManipulate(glm::value_ptr(m_EditorCamera.m_ViewMatrix), 8.0f, ImVec2(ImGui::GetWindowPos().x + (ImGui::GetWindowWidth() - 256), ImGui::GetWindowPos().y), ImVec2(256, 256), 0xff101010);

            {
                /*
                float ar = (float)ImGui::GetWindowHeight() / (float)ImGui::GetWindowWidth();
                float compassDist = 2;
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x + (ImGui::GetWindowWidth() - 256), ImGui::GetWindowPos().y + (ImGui::GetWindowHeight() - 256 * ar), 256, 256 * ar);

                glm::quat camRot = glm::quat(glm::vec3(-m_EditorCamera.m_Pitch, -m_EditorCamera.m_Yaw, 0.0f));
                glm::mat4 view = glm::inverse(glm::translate(glm::mat4(1.0f), -glm::rotate(camRot, glm::vec3(0.0f, 0.0f, -1.0f)) * compassDist) * glm::toMat4(camRot));
                glm::mat4 m = glm::mat4(1.0f);
                //ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(m_EditorCamera.m_ProjectionMatrix), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(m), nullptr, nullptr);
                ImGuizmo::DrawCube(glm::value_ptr(view), glm::value_ptr(m_EditorCamera.m_ProjectionMatrix), glm::value_ptr(m));
                */

            }


            // Gizmos
            if (m_GizmoType != -1 && m_SelectedEntity)//m_SelectionContext.size())
            {
                //auto& selection = m_SelectionContext[0];

                /*
                float rw = (float)ImGui::GetWindowWidth();
                float rh = (float)ImGui::GetWindowHeight();
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::SetDrawlist();
                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);
                */



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

                auto& entityTransform = m_SelectedEntity.Transform();
                float snapValue[3] = { m_SnapValue, m_SnapValue, m_SnapValue };
                
                //if (m_SelectionMode == SelectionMode::Entity)
                {
                    ImGuizmo::Manipulate(
                        glm::value_ptr(m_EditorCamera.m_ViewMatrix),
                        glm::value_ptr(m_EditorCamera.m_ProjectionMatrix),

                        //ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()),
                        //    glm::value_ptr(camera.GetProjectionMatrix()),
                        (ImGuizmo::OPERATION)m_GizmoType,
                        ImGuizmo::LOCAL,
                        glm::value_ptr(entityTransform),
                        nullptr,
                        snap ? snapValue : nullptr);
                }







                
                
                //else
                //{
                //    glm::mat4 transformBase = entityTransform * selection.Mesh->Transform;


                //    ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                //        glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),

                //        /*ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrix()),
                //            glm::value_ptr(camera.GetProjectionMatrix()),*/
                //        (ImGuizmo::OPERATION)m_GizmoType,
                //        ImGuizmo::LOCAL,
                //        glm::value_ptr(transformBase),
                //        nullptr,
                //        snap ? snapValue : nullptr);

                //    selection.Mesh->Transform = glm::inverse(entityTransform) * transformBase;
                //}

            }

            if (showStats)
            {
                m_StatsWindow.DrawWindow(viewportOffset, viewportSize, 300, 225, 10);
            }


            ImGui::End();
        }
        ImGui::PopStyleVar();
    }

    // camera straighten angles and focus

    void EditorLayer::DrawEditorPreferencesWindow()
    {
        ImGui::Begin("Editor Settings:");

        if (ImGui::CollapsingHeader("Scene View AABB", nullptr, ImGuiTreeNodeFlags_Selected))
        {
            EditorGUI::Color4Field("Gizmo Color", SceneRenderer::GetOptions().AABBColor);
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
            EditorGUI::DrawEditorColorPickers();
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
        //m_SceneHierarchyPanel->OnImGuiRender();

        //bool selectionChanged;
        //Entity deletedEntity;
        SceneHierarchyPanel::Draw(m_EditorScene, m_SelectedEntity, m_SceneHierarchyFocused);// , deletedEntity);

        


        /*if (selectionChanged)
        {
            SelectEntity(m_SelectedEntity);
        }
        if (deletedEntity)
        {
            OnEntityDeleted(deletedEntity);
        }*/



        ImGui::Begin("Inspector");



        ImGui::Separator();
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
        ImGui::End();


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
            if (ImGui::Button("Load Environment Map"))
            {
                std::string filename = Application::Get().OpenFile("*.hdr");
                if (filename != "")
                    m_EditorScene->SetEnvironment(Environment::Load(filename));
            }
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
        dispatcher.Dispatch<MouseButtonPressedEvent>(ARES_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        //auto [mx, my] = Input::GetMousePosition();
        auto mp = Input::GetMousePosition();
        float mx = mp.x;
        float my = mp.y;
        if (e.GetMouseButton() == MouseButtonCode::ButtonLeft && !Input::GetKey(KeyCode::LeftAlt) && !ImGuizmo::IsOver() && m_SceneState != SceneState::Play)
        {
            auto [mouseX, mouseY] = GetMouseViewportSpace();
            if (mouseX > -1.0f && mouseX < 1.0f && mouseY > -1.0f && mouseY < 1.0f)
            {
                auto [origin, direction] = CastRay(mouseX, mouseY);

                //m_SelectedSubmeshes.clear();

                //m_ActiveScene->m_Registry.each([&](auto entity)

                std::vector<SelectedSubmesh> m_SelectionContext;
                //m_SelectionContext.clear();
                //auto meshEntities = m_ActiveScene->GetAllEntitiesWith<MeshRendererComponent>();
                
                //m_EditorScene->SetSelectedEntity({});
                m_SelectedEntity = {};

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
                    //constexpr float lastT = std::numeric_limits<float>::max();
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
                                    //m_SelectionContext.push_back({ entity, &submesh, t });
                                    m_SelectionContext.push_back({ entity, t });

                                    break;
                                }
                            }
                        }
                    }
                }//);

                std::sort(m_SelectionContext.begin(), m_SelectionContext.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
                if (m_SelectionContext.size())
                {
                    //OnSelected(m_SelectionContext[0]);
                    m_SelectedEntity = m_SelectionContext[0].Entity;
                }

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

        auto inverseProj = glm::inverse(m_EditorCamera.m_ProjectionMatrix);
        auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.m_ViewMatrix));

        /*auto inverseProj = glm::inverse(m_CameraEntity.GetComponent<CameraComponent>().Camera.GetProjectionMatrix());
        auto inverseView = glm::inverse(glm::mat3(m_CameraEntity.GetComponent<CameraComponent>().Camera.GetViewMatrix()));*/

        /*auto inverseProj = glm::inverse(m_Scene->GetCamera().GetProjectionMatrix());
        auto inverseView = glm::inverse(glm::mat3(m_Scene->GetCamera().GetViewMatrix()));*/

        glm::vec4 ray = inverseProj * mouseClipPos;

        //glm::vec3 rayPos = m_Scene->GetCamera().GetPosition();
        //glm::vec3 rayPos = m_CameraEntity.GetComponent<CameraComponent>().Camera.GetPosition();
        glm::vec3 rayPos = glm::inverse(m_EditorCamera.m_ViewMatrix)[3];//.m_Position;

        glm::vec3 rayDir = inverseView * glm::vec3(ray);

        return { rayPos, rayDir };
    }

    //void EditorLayer::OnSelected(const SelectedSubmesh& selectionContext)
    //{
        //m_SceneHierarchyPanel->SetSelected(selectionContext.Entity);
        //m_EditorScene->SetSelectedEntity(selectionContext.Entity);
    //}
    /*
    void EditorLayer::OnEntityDeleted(Entity e)
    {
        if (m_SelectionContext[0].Entity == e)
        {
            m_SelectionContext.clear();
            m_EditorScene->SetSelectedEntity({});
        }
    }
    */


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

