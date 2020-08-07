#include "EditorLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "EditorGUI.h"
#include "Ares/ImGui/ImGuizmo.h"


#define _2D 0

namespace Ares
{

    EditorLayer::EditorLayer()
        : Layer("Sandbox2D"), 
        m_CameraController(1280.0f / 720.0f),
        m_SceneType(SceneType::Model)//,
        //m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
    {
    }
    void EditorLayer::OnAttach()
    {
        // ImGui Colors
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





        memset(m_FrameTimeGraph, 0, sizeof(float) * 100);

#if _2D
        // create a frame buffer
        FrameBufferSpecs fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Ares::FrameBuffer::Create(fbSpec, FramebufferFormat::RGBA8);

        // create scene
        m_ActiveScene = CreateRef<Scene>();
        
        // create sprite entity to draw
        {
            m_SquareEntity = m_ActiveScene->CreateEntity("Custom Entity");
            // add a sprite renderer component
            SpriteRendererComponent& spriteRenderer = m_SquareEntity.AddComponent<SpriteRendererComponent>();
            Ref<Texture2D> spriteSheet = Texture2D::Create("Assets/Textures/RPGpack_sheet_2X.png");
            spriteRenderer.Texture = spriteSheet;
        }

        // create a camera entity
        {
            m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
            // add a camera component
            m_CameraEntity.AddComponent<CameraComponent>();
        }

#else

        auto _ = Shader::Find("Assets/Shaders/pbr_anim.glsl");


        auto environment = Environment::Load("Assets/Textures/birchwood_4k.hdr");

        auto skyboxShader = Shader::Find("Assets/Shaders/CubemapSkybox.glsl");
        auto skyboxMaterial = CreateRef<MaterialInstance>(CreateRef<Material>(skyboxShader));
        skyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);

        // Model Scene
        {
            m_Scene = CreateRef<Scene>("Model Scene");
            m_Scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));
            m_Scene->SetSkyboxMaterial(skyboxMaterial);
            m_Scene->SetEnvironment(environment);

            m_MeshEntity = m_Scene->CreateEntity();
            MeshRendererComponent& mrComponent = m_MeshEntity.AddComponent<MeshRendererComponent>();
            //mrComponent.Mesh = nullptr;
            //mrComponent.MaterialInstances = nullptr;
            m_MeshMaterial = nullptr;
        }

            


        /*Ref<Shader> m_PBRShaderAnim = Shader::Find("Assets/Shaders/pbr_anim.glsl");
        Ref<Shader> m_PBRShaderStatic = Shader::Find("Assets/Shaders/pbr_static.glsl");*/
        
        /*m_GridMaterial = CreateRef<Material>(Shader::Find("Assets/Shaders/grid.glsl"));
        m_SkyboxShader = Shader::Find("Assets/Shaders/CubemapSkybox.glsl");

        m_HDRShader = Shader::Find("Assets/Shaders/hdr.glsl");

        m_PlaneMesh = CreateRef<Mesh>(PrimitiveType::Plane);
        m_CubeMesh = CreateRef<Mesh>(PrimitiveType::Cube);*/

        // Editor
        m_CheckerboardTex = Texture2D::Create("Assets/Textures/Checkerboard.png");

        // Environment
        /*m_EnvironmentCubeMap = TextureCube::Create("Assets/Textures/Arches_E_PineTree_Radiance.tga", false);
        m_EnvironmentIrradiance = TextureCube::Create("Assets/Textures/Arches_E_PineTree_Irradiance.tga", false);
        m_BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga");*/
        

        /*{
            FrameBufferSpecs fbSpec;
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            fbSpec.Format = FramebufferFormat::RGBA16F;
            fbSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

            RenderPassSpecs rpSpecs;
            rpSpecs.TargetFrameBuffer = FrameBuffer::Create(fbSpec);
            m_GeoPass = CreateRef<RenderPass>(rpSpecs);
        }
        {
            FrameBufferSpecs fbSpec;
            fbSpec.Width = 1280;
            fbSpec.Height = 720;
            fbSpec.Format = FramebufferFormat::RGBA8;
            fbSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

            RenderPassSpecs rpSpecs;
            rpSpecs.TargetFrameBuffer = FrameBuffer::Create(fbSpec);
            m_CompositePass = CreateRef<RenderPass>(rpSpecs);
        }*/

        // set up pbr materials for demo
        /*m_PBRMaterialStatic = CreateRef<Material>(m_PBRShaderStatic);
        m_PBRMaterialAnim = CreateRef<Material>(m_PBRShaderAnim);*/

        {

            m_SpheresScene = CreateRef<Scene>("PBR Sphere Scene");
            m_SpheresScene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));
            
            m_SpheresScene->SetSkyboxMaterial(skyboxMaterial);
            
            m_SpheresScene->SetEnvironment(environment);

            auto sphereMesh = CreateRef<Mesh>(PrimitiveMeshType::Cube);
            m_SphereBaseMaterial = sphereMesh->GetMaterial();

            float x = -8.0f;
            float roughness = 0.0f;
            for (int i = 0; i < 8; i++)
            {

                auto sphereEntity = m_SpheresScene->CreateEntity();
                MeshRendererComponent& mrComponent = sphereEntity.AddComponent<MeshRendererComponent>();

                Ref<MaterialInstance> mi = CreateRef<MaterialInstance>(m_SphereBaseMaterial);
                mi->Set("u_Metalness", 1.0f);
                mi->Set("u_Roughness", roughness);
                //mi->Set("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f)));
                x += 2;
                roughness += 0.15f;
                //m_MetalSphereMaterialInstances.push_back(mi);


                mrComponent.Mesh = sphereMesh;
                mrComponent.MaterialOverrides = { mi };
                sphereEntity.Transform() = glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f));
            }

            x = -8.0f;
            roughness = 0.0f;
            for (int i = 0; i < 8; i++)
            {
                auto sphereEntity = m_SpheresScene->CreateEntity();
                MeshRendererComponent& mrComponent = sphereEntity.AddComponent<MeshRendererComponent>();

                Ref<MaterialInstance> mi = CreateRef<MaterialInstance>(m_SphereBaseMaterial);
                mi->Set("u_Metalness", 0.0f);
                mi->Set("u_Roughness", roughness);
                //mi->Set("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 2, 0)));
                x += 2;
                roughness += 0.15f;
                //m_DielectricSphereMaterialInstances.push_back(mi);


                mrComponent.Mesh = sphereMesh;
                mrComponent.MaterialOverrides = { mi };
                sphereEntity.Transform() = glm::translate(glm::mat4(1.0f), glm::vec3(x, 2.0f, 0.0f));
            }
        }

        // Create Quad
        /*float* data = new float[4 * 2] {
            -1, -1,
             1, -1,
             1,  1,
            -1,  1
        };

        m_FullScreenQuadVAO = VertexArray::Create();

        Ref<VertexBuffer> quadVertexBuffer = VertexBuffer::Create(data, 4 * 2 * sizeof(float));
        quadVertexBuffer->SetLayout({
            { ShaderDataType::Float2, "a_Position" },
        });
        m_FullScreenQuadVAO->AddVertexBuffer(quadVertexBuffer);

        uint32_t* quadIndicies = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
        m_FullScreenQuadVAO->SetIndexBuffer(IndexBuffer::Create(quadIndicies, 6));
        delete[] quadIndicies;
        delete[] data;*/




        m_ActiveScene = m_Scene;
        m_SceneHierarchyPanel = CreateScope<SceneHierarchyPanel>(m_ActiveScene);

        m_Light.Direction = { -0.5f, -0.5f, 1.0f };
        m_Light.Radiance = { 1.0f, 1.0f, 1.0f };

        //m_Transform = glm::scale(glm::mat4(1.0f), glm::vec3(m_MeshScale));
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


    void EditorLayer::SetPBRMaterialValues(Ref<Material> material) const//, const glm::mat4& viewProjection) const
    {
        material->Set("u_AlbedoColor", m_AlbedoInput.Color);
        material->Set("u_Metalness", m_MetalnessInput.Value);
        material->Set("u_Roughness", m_RoughnessInput.Value);
        //material->Set("u_ViewProjectionMatrix", viewProjection);
        //material->Set("u_ModelMatrix", glm::scale(glm::mat4(1.0f), glm::vec3(m_MeshScale)));
        material->Set("lights", m_Light);
        //material->Set("u_CameraPosition", m_Camera.GetPosition());
        material->Set("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
        material->Set("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
        material->Set("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
        material->Set("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
        material->Set("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
        material->Set("u_EnvMapRotation", m_EnvMapRotation);

        //material->Set("u_EnvRadianceTex", m_EnvironmentCubeMap);
        //material->Set("u_EnvIrradianceTex", m_EnvironmentIrradiance);
        //material->Set("u_BRDFLUTTexture", m_BRDFLUT);

        if (m_AlbedoInput.TextureMap)    material->Set("u_AlbedoTexture", m_AlbedoInput.TextureMap);
        if (m_NormalInput.TextureMap)    material->Set("u_NormalTexture", m_NormalInput.TextureMap);
        if (m_MetalnessInput.TextureMap) material->Set("u_MetalnessTexture", m_MetalnessInput.TextureMap);
        if (m_RoughnessInput.TextureMap) material->Set("u_RoughnessTexture", m_RoughnessInput.TextureMap);
    }


    void EditorLayer::OnUpdate()
    {

        // THINGS TO LOOK AT:
        // - BRDF LUT
        // - Cubemap mips and filtering
        // - Tonemapping and proper HDR pipeline
        
        //m_Camera.Update();

        /*auto viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();

        if (m_Mesh)
        {
            m_Mesh->OnUpdate();
        }*/


        //m_FrameBuffer->Bind();
        //Renderer::BeginRenderPass(m_GeoPass);
        //Renderer::Clear(1, 0, 1, 1);



        // draw skybox
        /*m_SkyboxShader->Bind();
        m_SkyboxShader->SetMat4("u_InverseVP", glm::inverse(viewProjection));
        m_SkyboxShader->SetInt("u_Texture", 0);
        m_EnvironmentIrradiance->Bind(0);
        m_FullScreenQuadVAO->Bind();
        Renderer::DrawIndexed(m_FullScreenQuadVAO->GetIndexBuffer()->GetCount(), false);*/

        //SetPBRMaterialValues(m_PBRMaterialStatic);// , viewProjection);
        //SetPBRMaterialValues(m_PBRMaterialAnim);// , viewProjection);

        if (m_MeshMaterial)
        {
            SetPBRMaterialValues(m_MeshMaterial);// , viewProjection);
        }
        SetPBRMaterialValues(m_SphereBaseMaterial);// , viewProjection);


        //if (m_SceneType == SceneType::Spheres)
        //{
        //    // Metals
        //    for (int i = 0; i < 8; i++)
        //    {
        //        m_MetalSphereMaterialInstances[i]->Bind();

        //        Renderer::SubmitMesh(m_CubeMesh, glm::mat4(1.0f), m_MetalSphereMaterialInstances[i]->GetShader());
        //        //m_CubeMesh->Render(m_MetalSphereMaterialInstances[i]->GetShader());
        //    }

        //    // Dielectrics
        //    for (int i = 0; i < 8; i++)
        //    {
        //        m_DielectricSphereMaterialInstances[i]->Bind();
        //        Renderer::SubmitMesh(m_CubeMesh, glm::mat4(1.0f), m_DielectricSphereMaterialInstances[i]->GetShader());
        //        //m_CubeMesh->Render(m_DielectricSphereMaterialInstances[i]->GetShader());

        //    }

        //}
        //else if (m_SceneType == SceneType::Model)
        //{
        //    if (m_Mesh)
        //    {
        //        Ref<Material> material = m_Mesh->IsAnimated() ? m_PBRMaterialAnim : m_PBRMaterialStatic;
        //        material->Bind();
        //        Renderer::SubmitMesh(m_Mesh, m_Transform, material->GetShader());
        //        //m_Mesh->Render(material->GetShader(), m_Transform);   
        //    }
        //}

        //m_GridMaterial->Set("u_MVP", viewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(m_GridScale - m_GridSize)));
        //m_GridMaterial->Set("u_Scale", (float)m_GridScale);
        //m_GridMaterial->Set("u_Res", m_GridSize);
        //m_GridMaterial->Bind();
        ////m_PlaneMesh->Render(nullptr);
        //Renderer::SubmitMesh(m_Mesh, glm::mat4(1.0f), nullptr);


        ///*m_FrameBuffer->Unbind();
        //m_FinalPresentBuffer->Bind();*/

        //Renderer::EndRenderPass();
        //Renderer::BeginRenderPass(m_CompositePass);

        //
        //m_HDRShader->Bind();
        //m_HDRShader->SetFloat("u_Exposure", m_Exposure);
        //m_HDRShader->SetInt("u_Texture", 0);
        //
        //// bind original frame buffer as textur 0
        ////m_FrameBuffer->BindAsTexture();
        //m_GeoPass->GetSpecs().TargetFrameBuffer->BindAsTexture();
        //
        //m_FullScreenQuadVAO->Bind();
        //Renderer::DrawIndexed(m_FullScreenQuadVAO->GetIndexBuffer()->GetCount(), false);

        ////m_FinalPresentBuffer->Unbind();
        //Renderer::EndRenderPass();

        if (m_AllowViewportCameraEvents)
            m_ActiveScene->GetCamera().Update();

        m_ActiveScene->OnUpdate();

        if (m_DrawOnTopBoundingBoxes)
        {
            Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
            auto viewProj = m_ActiveScene->GetCamera().GetViewProjection();
            Renderer2D::BeginScene(viewProj, false);
            // Renderer2D::DrawQuad({ 0, 0, 0 }, { 4.0f, 5.0f }, { 1.0f, 1.0f, 0.5f, 1.0f });
            
            auto mesh = m_MeshEntity.GetComponent<MeshRendererComponent>().Mesh;
            if (mesh)
            {
                Renderer::DrawAABB(mesh);
            }
            
            
            Renderer2D::EndScene();
            Renderer::EndRenderPass();
        }
    }

    void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
    {
        SceneRenderer::GetOptions().ShowBoundingBoxes = show && !onTop;
        m_DrawOnTopBoundingBoxes = show && onTop;
    }

#endif


    void EditorLayer::OnImGuiDraw()
    {
        static bool dockspaceOpen = true;

        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
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
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    Ares::Application::Get().Close();

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Docking"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows, 
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

                if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 
                    dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                
                    dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          
                    dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                ImGui::Separator();
                if (ImGui::MenuItem("Close DockSpace", NULL, false, dockspaceOpen != NULL))
                    dockspaceOpen = false;

            }
            EditorGUI::ShowTooltip(
                "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n\n"
                " > if io.ConfigDockingWithShift==false (default):" "\n"
                "   drag windows from title bar to dock" "\n"
                " > if io.ConfigDockingWithShift==true:" "\n"
                "   drag windows from anywhere and hold Shift to dock" "\n\n"
                "This demo app has nothing to do with it!" "\n\n"
                "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)." "\n\n"
                "ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame." "\n\n"
                "(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
            );
            

            ImGui::EndMenuBar();
        }


        ImGui::Begin("Renderer Performance:");
        {
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
                float orthoSize = cameraComponent.Camera.GetOrthoSize();

                if (ImGui::SliderFloat("Camera Ortho Size", &orthoSize, .001f, 10))
                {
                    cameraComponent.Camera.SetOrthoSize(orthoSize);
                }

                ImGui::Separator();
            }
            
            ImGui::End();
        }

#else

        // Editor Panel ------------------------------------------------------------------------------
        ImGui::Begin("Settings");
        
        if (ImGui::RadioButton("Spheres", (int*)&m_SceneType, (int)SceneType::Spheres))
            m_ActiveScene = m_SpheresScene;
        ImGui::SameLine();
        if (ImGui::RadioButton("Model", (int*)&m_SceneType, (int)SceneType::Model))
            m_ActiveScene = m_Scene;

        ImGui::Begin("Environment");

        if (ImGui::Button("Load Environment Map"))
        {
            std::string filename = Application::Get().OpenFile("*.hdr");
            if (filename != "")
                m_ActiveScene->SetEnvironment(Environment::Load(filename));
        }

        ImGui::SliderFloat("Skybox LOD", &m_ActiveScene->GetSkyboxLod(), 0.0f, 11.0f);


        ImGui::Columns(2);
        ImGui::AlignTextToFramePadding();


        EditorGUI::Vec3("Light Direction", m_Light.Direction);
        EditorGUI::Color3("Light Radiance", m_Light.Radiance);
        EditorGUI::FloatSlider("Light Multiplier", m_LightMultiplier, 0.0f, 5.0f);


        EditorGUI::FloatSlider("Exposure", m_ActiveScene->GetExposure(), 0.0f, 5.0f);
        //EditorGUI::FloatSlider("Exposure", m_Exposure, 0.0f, 5.0f);

        //EditorGUI::FloatSlider("Mesh Scale", m_MeshScale, 0.0f, 2.0f);

        /*EditorGUI::IntSlider("Grid Scale", m_GridScale, 0, 20);
        EditorGUI::FloatSlider("Grid Size", m_GridSize, 0.0f, 0.1f);*/

        EditorGUI::Toggle("Radiance Prefiltering", m_RadiancePrefilter);
        EditorGUI::FloatSlider("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f);

        if (EditorGUI::Toggle("Show Bounding Boxes", m_UIShowBoundingBoxes))
            ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
        if (m_UIShowBoundingBoxes && EditorGUI::Toggle("On Top", m_UIShowBoundingBoxesOnTop))
            ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);

        //EditorGUI::Color4("Test Color", testColor);

        ImGui::Columns(1);

        ImGui::End();


        ImGui::Separator();
        {
            ImGui::Text("Mesh");

            auto mesh = m_MeshEntity.GetComponent<MeshRendererComponent>().Mesh;
            //std::string fullpath = m_Mesh ? m_Mesh->GetFilePath() : "None";
            std::string fullpath = mesh ? mesh->GetFilePath() : "None";

            size_t found = fullpath.find_last_of("/\\");
            std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
            ImGui::Text(path.c_str()); ImGui::SameLine();
            if (ImGui::Button("...##Mesh"))
            {
                std::string filename = Application::Get().OpenFile("");
                if (filename != "")
                {
                    MeshRendererComponent& mrComponent = m_MeshEntity.GetComponent<MeshRendererComponent>();
                    mrComponent.Mesh = CreateRef<Mesh>(filename);
                    // set material from load..
                    m_MeshMaterial = mrComponent.Mesh->GetMaterial();
                    //m_Mesh = CreateRef<Mesh>(filename);
                }
            }
        }
        ImGui::Separator();

        // Textures ------------------------------------------------------------------------------
        {
            // Albedo
            if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                ImGui::Image(m_AlbedoInput.TextureMap ? (void*)m_AlbedoInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
                ImGui::PopStyleVar();
                if (ImGui::IsItemHovered())
                {
                    if (m_AlbedoInput.TextureMap)
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted(m_AlbedoInput.TextureMap->GetPath().c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::Image((void*)m_AlbedoInput.TextureMap->GetRendererID(), ImVec2(384, 384));
                        ImGui::EndTooltip();
                    }
                    if (ImGui::IsItemClicked())
                    {
                        std::string filename = Application::Get().OpenFile("");
                        if (filename != "")
                            m_AlbedoInput.TextureMap = Texture2D::Create(filename, m_AlbedoInput.SRGB);
                    }
                }
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Checkbox("Use##AlbedoMap", &m_AlbedoInput.UseTexture);
                if (ImGui::Checkbox("sRGB##AlbedoMap", &m_AlbedoInput.SRGB))
                {
                    if (m_AlbedoInput.TextureMap)
                        m_AlbedoInput.TextureMap = Texture2D::Create(m_AlbedoInput.TextureMap->GetPath(), m_AlbedoInput.SRGB);
                }
                ImGui::EndGroup();
                ImGui::SameLine();
                ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(m_AlbedoInput.Color), ImGuiColorEditFlags_NoInputs);
            }
        }
        {
            // Normals
            if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                ImGui::Image(m_NormalInput.TextureMap ? (void*)m_NormalInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
                ImGui::PopStyleVar();
                if (ImGui::IsItemHovered())
                {
                    if (m_NormalInput.TextureMap)
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted(m_NormalInput.TextureMap->GetPath().c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::Image((void*)m_NormalInput.TextureMap->GetRendererID(), ImVec2(384, 384));
                        ImGui::EndTooltip();
                    }
                    if (ImGui::IsItemClicked())
                    {
                        std::string filename = Application::Get().OpenFile("");
                        if (filename != "")
                            m_NormalInput.TextureMap = Texture2D::Create(filename);
                    }
                }
                ImGui::SameLine();
                ImGui::Checkbox("Use##NormalMap", &m_NormalInput.UseTexture);
            }
        }
        {
            // Metalness
            if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                ImGui::Image(m_MetalnessInput.TextureMap ? (void*)m_MetalnessInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
                ImGui::PopStyleVar();
                if (ImGui::IsItemHovered())
                {
                    if (m_MetalnessInput.TextureMap)
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted(m_MetalnessInput.TextureMap->GetPath().c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::Image((void*)m_MetalnessInput.TextureMap->GetRendererID(), ImVec2(384, 384));
                        ImGui::EndTooltip();
                    }
                    if (ImGui::IsItemClicked())
                    {
                        std::string filename = Application::Get().OpenFile("");
                        if (filename != "")
                            m_MetalnessInput.TextureMap = Texture2D::Create(filename);
                    }
                }
                ImGui::SameLine();
                ImGui::Checkbox("Use##MetalnessMap", &m_MetalnessInput.UseTexture);
                ImGui::SameLine();
                ImGui::SliderFloat("Value##MetalnessInput", &m_MetalnessInput.Value, 0.0f, 1.0f);
            }
        }
        {
            // Roughness
            if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                ImGui::Image(m_RoughnessInput.TextureMap ? (void*)m_RoughnessInput.TextureMap->GetRendererID() : (void*)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));
                ImGui::PopStyleVar();
                if (ImGui::IsItemHovered())
                {
                    if (m_RoughnessInput.TextureMap)
                    {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                        ImGui::TextUnformatted(m_RoughnessInput.TextureMap->GetPath().c_str());
                        ImGui::PopTextWrapPos();
                        ImGui::Image((void*)m_RoughnessInput.TextureMap->GetRendererID(), ImVec2(384, 384));
                        ImGui::EndTooltip();
                    }
                    if (ImGui::IsItemClicked())
                    {
                        std::string filename = Application::Get().OpenFile("");
                        if (filename != "")
                            m_RoughnessInput.TextureMap = Texture2D::Create(filename);
                    }
                }
                ImGui::SameLine();
                ImGui::Checkbox("Use##RoughnessMap", &m_RoughnessInput.UseTexture);
                ImGui::SameLine();
                ImGui::SliderFloat("Value##RoughnessInput", &m_RoughnessInput.Value, 0.0f, 1.0f);
            }
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
#endif

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        auto viewportSize = ImGui::GetContentRegionAvail();


#if _2D
        m_ViewportSize = { viewportSize.x, viewportSize.y };
        ImGui::Image((void*)m_FrameBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
#else

        SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        m_ActiveScene->GetCamera().SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        m_ActiveScene->GetCamera().SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
        ImGui::Image((void*)SceneRenderer::GetFinalColorBufferRendererID(), viewportSize, { 0, 1 }, { 1, 0 });


        /*m_GeoPass->GetSpecs().TargetFrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        m_CompositePass->GetSpecs().TargetFrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);*/
        
        /*m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        m_FinalPresentBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);*/
        
        /*m_Camera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        m_Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));*/

        //ImGui::Image((void*)m_FinalPresentBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
        //ImGui::Image((void*)m_CompositePass->GetSpecs().TargetFrameBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });


        static int counter = 0;
        auto windowSize = ImGui::GetWindowSize();
        ImVec2 minBound = ImGui::GetWindowPos();
        ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
        m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

#endif

        // Gizmos
        if (m_GizmoType != -1)
        {
            float rw = (float)ImGui::GetWindowWidth();
            float rh = (float)ImGui::GetWindowHeight();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);
            //ImGuizmo::Manipulate(glm::value_ptr(m_Camera.GetViewMatrix()), glm::value_ptr(m_Camera.GetProjectionMatrix()), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(m_Transform));
            ImGuizmo::Manipulate(
                glm::value_ptr(m_ActiveScene->GetCamera().GetViewMatrix()), // view
                glm::value_ptr(m_ActiveScene->GetCamera().GetProjectionMatrix()), // projection
                (ImGuizmo::OPERATION)m_GizmoType, // operation
                ImGuizmo::LOCAL, // mode
                glm::value_ptr(m_MeshEntity.Transform())
            );

        }


        ImGui::End();
        ImGui::PopStyleVar();

        m_SceneHierarchyPanel->OnImGuiRender();

        ImGui::End();

        /*if (m_Mesh)
            m_Mesh->OnImGuiRender();*/
    }

    void EditorLayer::OnEvent(Ares::Event& e)
    {
        m_CameraController.OnEvent(e);

        if (m_AllowViewportCameraEvents)
            m_ActiveScene->GetCamera().OnEvent(e);

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(ARES_BIND_EVENT_FN(EditorLayer::OnKeyPressedEvent));
    }
    bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& e)
    {
        switch (e.GetKeyCode())
        {
        case ARES_KEY_Q:
            m_GizmoType = -1;
            break;
        case ARES_KEY_W:
            m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case ARES_KEY_E:
            m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        case ARES_KEY_R:
            m_GizmoType = ImGuizmo::OPERATION::SCALE;
            break;

        case ARES_KEY_G:
            // Toggle grid
            if (Input::IsKeyPressed(ARES_KEY_LEFT_CONTROL))
                SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
            break;
        case ARES_KEY_B:
            // Toggle bounding boxes 
            if (Input::IsKeyPressed(ARES_KEY_LEFT_CONTROL))
            {
                m_UIShowBoundingBoxes = !m_UIShowBoundingBoxes;
                ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
            }
            break;
        }
        return false;
    }


}
