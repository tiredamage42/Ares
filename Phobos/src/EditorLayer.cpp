#include "EditorLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ares
{

    EditorLayer::EditorLayer()
        : Layer("Sandbox2D"), 
        m_CameraController(1280.0f / 720.0f),
        m_Scene(Scene::Spheres),
        m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
    {
    }
    void EditorLayer::OnAttach()
    {
#if _2D
        FrameBufferSpecs fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Ares::FrameBuffer::Create(fbSpec);

        memset(m_FrameTimeGraph, 0, sizeof(float) * 100);

        m_ActiveScene = CreateRef<Scene>();
        
        auto square = m_ActiveScene->CreateEntity("Custom Entity");
       
        Ref<Texture2D> spriteSheet = Texture2D::Create("Assets/Game/Textures/RPGpack_sheet_2X.png");

        SpriteRendererComponent& spriteRenderer = square.AddComponent<SpriteRendererComponent>();        
        spriteRenderer.Color = glm::vec4{ 0, 1, 0, 1 };
        spriteRenderer.Texture = spriteSheet;

        m_SquareEntity = square;
#else
        m_SimplePBRShader = Shader::Create("Assets/Shaders/pbr.glsl");
        
        //m_QuadShader = Shader::Create("assets/shaders/quad.glsl");
        m_QuadShader = Shader::Create("Assets/Shaders/CubemapSkybox.glsl");

        m_HDRShader = Shader::Create("Assets/Shaders/hdr.glsl");


        //m_Mesh = Mesh::Create("assets/meshes/cerberus.fbx");

        m_Mesh = CreateRef<Mesh>();// "Assets/Models/m1911/m1911.fbx");
        m_SphereMesh = CreateRef<Mesh>();// "assets/models/Sphere.fbx");

        // Editor
        m_CheckerboardTex = Texture2D::Create("Assets/Textures/Checkerboard.png");

        // Environment
        m_EnvironmentCubeMap = TextureCube::Create("Assets/Textures/Arches_E_PineTree_Radiance.tga", false);
        m_EnvironmentIrradiance = TextureCube::Create("Assets/Textures/Arches_E_PineTree_Irradiance.tga", false);
        //m_EnvironmentIrradiance = TextureCube::Create("Assets/Textures/Arches_E_PineTree_Radiance.tga", false);
        

        m_BRDFLUT = Texture2D::Create("Assets/Textures/BRDF_LUT.tga");


        FrameBufferSpecs fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;

        m_FrameBuffer = FrameBuffer::Create(fbSpec, FramebufferFormat::RGBA16F);
        m_FinalPresentBuffer = FrameBuffer::Create(fbSpec, FramebufferFormat::RGBA8);

        // Create Quad
        float x = -1, y = -1;
        float width = 2, height = 2;
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec2 TexCoord;
        };

        QuadVertex* data = new QuadVertex[4];

        data[0].Position = glm::vec3(x, y, 0);
        data[0].TexCoord = glm::vec2(0, 0);

        data[1].Position = glm::vec3(x + width, y, 0);
        data[1].TexCoord = glm::vec2(1, 0);

        data[2].Position = glm::vec3(x + width, y + height, 0);
        data[2].TexCoord = glm::vec2(1, 1);

        data[3].Position = glm::vec3(x, y + height, 0);
        data[3].TexCoord = glm::vec2(0, 1);

        /*m_VertexBuffer = VertexBuffer::Create(4 * sizeof(QuadVertex));
        m_VertexBuffer->SetData(data, 4 * sizeof(QuadVertex));

        uint32_t* indices = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
        m_IndexBuffer = IndexBuffer::Create(indices, 6);*/
        //m_IndexBuffer->SetData(indices, 6 * sizeof(unsigned int));


        

        m_QuadVertexArray = VertexArray::Create();

        // VERTEX BUFFER =============================================================================
        Ref<VertexBuffer> quadVertexBuffer = VertexBuffer::Create(4 * sizeof(QuadVertex));
        quadVertexBuffer->SetData(data, 4 * sizeof(QuadVertex));

        quadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });

        m_QuadVertexArray->AddVertexBuffer(quadVertexBuffer);

        // INDEX BUFFER =============================================================================
        uint32_t* quadIndicies = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndicies, 6);
        m_QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndicies;




















        m_Light.Direction = { -0.5f, -0.5f, 1.0f };
        m_Light.Radiance = { 1.0f, 1.0f, 1.0f };
#endif
    }

    void EditorLayer::OnDetach()
    {
        
    }

#if _2D
    void EditorLayer::OnUpdate()
    {
        
        // Resize
        /*
            This solution will render the 'old' sized framebuffer onto the 'new' sized ImGuiPanel
            and store the 'new' size in m_ViewportSize.
            The next frame will first resize the framebuffer as m_ViewportSize differs
            from m_Framebuffer.Width/Height before updating and rendering.
            This results in never rendering an empty (black) framebuffer.
        */
        // zero sized framebuffer is invalid
        /*if (m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y))
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);*/
        
        if (m_ViewportFocused)
            m_CameraController.OnUpdate();

        // render
        m_FrameBuffer->Bind();

        Renderer::Clear(.1f, .1f, .1f, 1);
            
        Renderer2D::BeginScene(m_CameraController.GetCamera());

        // update scnee
        m_ActiveScene->OnUpdate();

        Renderer2D::DrawQuad(
            { 0.0f, 0.0f, 0.1f }, glm::radians(-45.0f), { 0.5f, 0.5f }, 
            nullptr, glm::vec2(1.0f), glm::vec2(0.0f), { 1.0f, 0.0f, 1.0f, 1.0f }
        );
        
        Renderer2D::EndScene();

        m_FrameBuffer->Unbind();
    }
#else
    void EditorLayer::OnUpdate()
    {

        // THINGS TO LOOK AT:
        // - BRDF LUT
        // - Cubemap mips and filtering
        // - Tonemapping and proper HDR pipeline
        

        if (m_ViewportFocused)
            m_Camera.Update();

        auto viewProjection = m_Camera.GetProjectionMatrix() * m_Camera.GetViewMatrix();


        m_FrameBuffer->Bind();
        //m_FinalPresentBuffer->Bind();

        Renderer::Clear(1, 0, 1, 1);


        // draw skybox
        m_QuadShader->Bind();

        /*UniformBufferDeclaration<sizeof(glm::mat4), 1> quadShaderUB;
        quadShaderUB.Push("u_InverseVP", glm::inverse(viewProjection));
        m_QuadShader->UploadUniformBuffer(quadShaderUB);*/

        m_QuadShader->SetMat4("u_InverseVP", glm::inverse(viewProjection));
        m_QuadShader->SetInt("u_Texture", 0);

        m_EnvironmentIrradiance->Bind(0);

        m_QuadVertexArray->Bind();
        /*m_VertexBuffer->Bind();
        m_IndexBuffer->Bind();*/
        //Renderer::DrawIndexed(m_IndexBuffer->GetCount(), false);
        Renderer::DrawIndexed(m_QuadVertexArray->GetIndexBuffer()->GetCount(), false);




        m_SimplePBRShader->Bind();

        UniformBufferDeclaration<sizeof(glm::mat4) * 2 + sizeof(glm::vec3) * 4 + sizeof(float) * 8, 14> simplePbrShaderUB;
        simplePbrShaderUB.Push("u_ViewProjectionMatrix", viewProjection);
        simplePbrShaderUB.Push("u_ModelMatrix", glm::mat4(1.0f));
        simplePbrShaderUB.Push("u_AlbedoColor", m_AlbedoInput.Color);
        simplePbrShaderUB.Push("u_Metalness", m_MetalnessInput.Value);
        simplePbrShaderUB.Push("u_Roughness", m_RoughnessInput.Value);
        simplePbrShaderUB.Push("lights.Direction", m_Light.Direction);
        simplePbrShaderUB.Push("lights.Radiance", m_Light.Radiance * m_LightMultiplier);
        simplePbrShaderUB.Push("u_CameraPosition", m_Camera.GetPosition());
        simplePbrShaderUB.Push("u_RadiancePrefilter", m_RadiancePrefilter ? 1.0f : 0.0f);
        simplePbrShaderUB.Push("u_AlbedoTexToggle", m_AlbedoInput.UseTexture ? 1.0f : 0.0f);
        simplePbrShaderUB.Push("u_NormalTexToggle", m_NormalInput.UseTexture ? 1.0f : 0.0f);
        simplePbrShaderUB.Push("u_MetalnessTexToggle", m_MetalnessInput.UseTexture ? 1.0f : 0.0f);
        simplePbrShaderUB.Push("u_RoughnessTexToggle", m_RoughnessInput.UseTexture ? 1.0f : 0.0f);
        simplePbrShaderUB.Push("u_EnvMapRotation", m_EnvMapRotation);
        m_SimplePBRShader->UploadUniformBuffer(simplePbrShaderUB);
        
        // PBR shader textures
        m_SimplePBRShader->SetInt("u_AlbedoTexture", 1);
        m_SimplePBRShader->SetInt("u_NormalTexture", 2);
        m_SimplePBRShader->SetInt("u_MetalnessTexture", 3);
        m_SimplePBRShader->SetInt("u_RoughnessTexture", 4);

        m_SimplePBRShader->SetInt("u_EnvRadianceTex", 10);
        m_SimplePBRShader->SetInt("u_EnvIrradianceTex", 11);
        m_SimplePBRShader->SetInt("u_BRDFLUTTexture", 15);

        if (m_AlbedoInput.TextureMap) m_AlbedoInput.TextureMap->Bind(1);
        if (m_NormalInput.TextureMap) m_NormalInput.TextureMap->Bind(2);
        if (m_MetalnessInput.TextureMap) m_MetalnessInput.TextureMap->Bind(3);
        if (m_RoughnessInput.TextureMap) m_RoughnessInput.TextureMap->Bind(4);
        m_EnvironmentCubeMap->Bind(10);
        m_EnvironmentIrradiance->Bind(11);
        m_BRDFLUT->Bind(15);


        if (m_Scene == Scene::Spheres)
        {
            // Metals
            float roughness = 0.0f;
            //float x = -88.0f;
            float x = -8;
            for (int i = 0; i < 8; i++)
            {
                //m_SimplePBRShader->SetMat4("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f)));
                m_SimplePBRShader->SetMat4("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.0f, 0.0f)));
                m_SimplePBRShader->SetFloat("u_Roughness", roughness);
                m_SimplePBRShader->SetFloat("u_Metalness", 1.0f);
                m_SphereMesh->Render();

                roughness += 0.15f;
                //x += 22.0f;
                x += 2;
            }

            // Dielectrics
            roughness = 0.0f;
            //x = -88.0f;
            x = -8;
            for (int i = 0; i < 8; i++)
            {
                //m_SimplePBRShader->SetMat4("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 22.0f, 0.0f)));
                m_SimplePBRShader->SetMat4("u_ModelMatrix", glm::translate(glm::mat4(1.0f), glm::vec3(x, 2, 0)));
                m_SimplePBRShader->SetFloat("u_Roughness", roughness);
                m_SimplePBRShader->SetFloat("u_Metalness", 0.0f);
                m_SphereMesh->Render();

                roughness += 0.15f;
                //x += 22.0f;
                x += 2;
            }

        }
        else if (m_Scene == Scene::Model)
        {
            m_Mesh->Render();
        }


        m_FrameBuffer->Unbind();
        m_FinalPresentBuffer->Bind();
        
        m_HDRShader->Bind();
        m_HDRShader->SetFloat("u_Exposure", m_Exposure);
        m_HDRShader->SetInt("u_Texture", 0);
        
        // bind original frame buffer as textur 0
        m_FrameBuffer->BindTexture();
        
        m_QuadVertexArray->Bind();
        //m_VertexBuffer->Bind();
        //m_IndexBuffer->Bind();
        Renderer::DrawIndexed(m_QuadVertexArray->GetIndexBuffer()->GetCount(), false);
        m_FinalPresentBuffer->Unbind();
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
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

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
            /*
            HelpMarker(
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
            */

            ImGui::EndMenuBar();
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
                
                ImGui::Separator();
            }
            
            ImGui::End();
        }
#endif

        ImGui::Begin("Renderer Performance:");
        {
            m_FrameTimeGraph[values_offset] = (float)(Time::GetDeltaTime() * 1000.0); // get in milliseconds
            values_offset = (values_offset + 1) % 100;

            ImGui::PlotLines("##Frametime", m_FrameTimeGraph, 100, values_offset, "Frametime (ms)", 0.0f, 66.6f, ImVec2(0, 100));
            ImGui::Text("Frametime: %.2fms", Time::GetDeltaTime() * 1000.0); // get in milliseconds
            ImGui::Text("FPS: %d", Time::GetFPS());

            auto& caps = RendererAPI::GetCapabilities();
            ImGui::Text("Vendor: %s", caps.Vendor.c_str());
            ImGui::Text("Renderer: %s", caps.Renderer.c_str());
            ImGui::Text("Version: %s", caps.Version.c_str());
            
            ImGui::End();
        }





















        // Editor Panel ------------------------------------------------------------------------------
        ImGui::Begin("Settings");
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

        ImGui::RadioButton("Spheres", (int*)&m_Scene, (int)Scene::Spheres);
        ImGui::SameLine();
        ImGui::RadioButton("Model", (int*)&m_Scene, (int)Scene::Model);

        ImGui::SliderFloat3("Light Dir", glm::value_ptr(m_Light.Direction), -1, 1);
        ImGui::ColorEdit3("Light Radiance", glm::value_ptr(m_Light.Radiance));
        ImGui::SliderFloat("Light Multiplier", &m_LightMultiplier, 0.0f, 5.0f);
        ImGui::SliderFloat("Exposure", &m_Exposure, 0.0f, 10.0f);

        auto cameraForward = m_Camera.GetForwardDirection();
        ImGui::Text("Camera Forward: %.2f, %.2f, %.2f", cameraForward.x, cameraForward.y, cameraForward.z);

        ImGui::Separator();
        {
            ImGui::Text("Mesh");
            std::string fullpath = m_Mesh ? m_Mesh->GetFilePath() : "None";
            size_t found = fullpath.find_last_of("/\\");
            std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
            ImGui::Text(path.c_str()); ImGui::SameLine();
            if (ImGui::Button("...##Mesh"))
            {
                std::string filename = Application::Get().OpenFile("");
                if (filename != "")
                {
                    //ARES_INFO("Would Load File '{0}'", filename);
                    m_Mesh = CreateRef<Mesh>(filename);
                }
            }
        }
        ImGui::Separator();

        ImGui::Text("Shader Parameters");
        ImGui::Checkbox("Radiance Prefiltering", &m_RadiancePrefilter);
        ImGui::SliderFloat("Env Map Rotation", &m_EnvMapRotation, -360.0f, 360.0f);

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

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        auto viewportSize = ImGui::GetContentRegionAvail();
        //m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
        m_FinalPresentBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

        m_Camera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));

        // from 2d on resize
        m_CameraController.OnResize(viewportSize.x, viewportSize.y);



        ImGui::Image((void*)m_FinalPresentBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
        ImGui::End();
        ImGui::PopStyleVar();



        /*ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
        ImGui::Begin("Viewport");
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportSize.x, viewportSize.y };

        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar();*/











        ImGui::End();

    }

    void EditorLayer::OnEvent(Ares::Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}
