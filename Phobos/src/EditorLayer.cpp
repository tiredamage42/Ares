#include "EditorLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Ares
{

    //static const uint32_t s_MapWidth = 24;
    //static const char* s_MapTiles = 
    //"WWWWWWWWWWWWWWWWWWWWWWWW"
    //"WWWWWWDDDDDDWWWWWWWWWWWW"
    //"WWWWDDDDDDDDDDDWWWWWWWWW"
    //"WWWWDDDDDDDDDDDDWWWWWWWW"
    //"WWWWDDDDWWDDDDDDDDWWWWWW"
    //"WWWWDDDDWWWDDDDDDDDWWWWW"
    //"WWWWDDDDDWWDDDDDDDWWWWWW"
    //"WWWWWDDDDDDDDDDWWWWWWWWW"
    //"WWWWWWDDDDDDDWWWWWWWWWWW"
    //"WWWWWWWWDDDDDWWWWWWWWWWW"
    //"WWWWWWWWWWWWWWWWWWWWWWWW"
    //;


    EditorLayer::EditorLayer()
        : Layer("Sandbox2D"),
        m_CameraController(1280.0f / 720.0f)
    {
    }
    void EditorLayer::OnAttach()
    {
        ARES_PROFILE_FUNCTION();


        m_Texture = Ares::Texture2D::Create("Assets/Textures/Checkerboard.png");

        //m_SpriteSheet = Ares::Texture2D::Create("Assets/Game/Textures/RPGpack_sheet_2X.png");
    
        /*m_TextureStairs = Ares::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7.0f, 6.0f }, { 128, 128 });
    
        m_TextureBarrel = Ares::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8.0f, 2.0f }, { 128, 128 });
        m_TextureTree = Ares::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2.0f, 1.0f }, { 128, 128 }, { 1,2 });*/

        /*s_TextureMap['D'] = Ares::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6.0f, 11.0f }, { 128, 128 });
        s_TextureMap['W'] = Ares::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11.0f, 11.0f }, { 128, 128 });
        */
        /*m_MapWidth = s_MapWidth;
        m_MapHeight = strlen(s_MapTiles) / m_MapWidth;*/


        /*m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
        m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
        m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
        m_Particle.LifeTime = 1.0f;
        m_Particle.Velocity = { 0.0f, 0.0f };
        m_Particle.VelocityVariation = { 3.0f, 1.0f };
        m_Particle.Position = { 0.0f, 0.0f };*/

        FrameBufferSpecification fbSpec;
        fbSpec.Width = 1280;
        fbSpec.Height = 720;
        m_FrameBuffer = Ares::FrameBuffer::Create(fbSpec);

    }
    void EditorLayer::OnDetach()
    {
        ARES_PROFILE_FUNCTION();

    }
    void EditorLayer::OnUpdate(float deltaTime)
    {
        ARES_PROFILE_FUNCTION();

        // Resize
        /*
        This solution will render the 'old' sized framebuffer onto the 'new' sized ImGuiPanel 
        and store the 'new' size in m_ViewportSize. 
        The next frame will first resize the framebuffer as m_ViewportSize differs 
        from m_Framebuffer.Width/Height before updating and rendering. 
        This results in never rendering an empty (black) framebuffer.
        */
        FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
        if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }


        if (m_ViewportFocused)
            m_CameraController.OnUpdate(deltaTime);
    
        // render
        Renderer2D::ResetStats();
        //Ares::Renderer2D::StatsBeginFrame();


        {
            ARES_PROFILE_SCOPE("Renderer Prep");

            m_FrameBuffer->Bind();

            Ares::RenderCommand::SetClearColor({ .1f, .1f, .1f, 1 });
            Ares::RenderCommand::Clear();
        }

        {
            ARES_PROFILE_SCOPE("Renderer Draw");
            Ares::Renderer2D::BeginScene(m_CameraController.GetCamera());

            Ares::Renderer2D::DrawQuad(
                { -1.0f, 0.0f }, glm::radians(-45.0f), { 0.8f, 0.8f }, 
                nullptr, 1.0f, { 0.8f, 0.2f, 0.3f, 1.0f }
            );
            Ares::Renderer2D::DrawQuad(
                { 0.5f, -0.5f }, 0.0f, { 0.5f, 0.75f }, 
                nullptr, 1.0f, { 0.2f, 0.3f, 0.8f, 1.0f }
            );
            Ares::Renderer2D::DrawQuad(
                { 0.0f, 0.0f, -0.1f }, 0.0f, { 10.0f, 10.0f }, 
                m_Texture, 10.0f
            );

            Ares::Renderer2D::DrawQuad(
                { -2.0f, 0.0f, 0.0f }, glm::radians(45.0f), { 1.0f, 1.0f },
                m_Texture, 10.0f
            );


            for (float y = -5.0; y < 5.0f; y += 0.5f)
            {
                for (float x = -5.0; x < 5.0f; x += 0.5f)
                {
                    glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 1.0f };
                    Ares::Renderer2D::DrawQuad({ x, y }, 0.0f, { 0.45f, 0.45f }, nullptr, 1.0f, color);
                }

            }


            /*Ares::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.75f, 0.75f }, { 0.8f, 0.2f, 0.3f, 1.0f });
            Ares::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
            Ares::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_Texture);*/
        
        
        
        
        
            Ares::Renderer2D::EndScene();

            m_FrameBuffer->Unbind();

            //Ares::Renderer2D::StatsEndFrame();





            /*if (Ares::Input::IsMouseButtonPressed(ARES_MOUSE_BUTTON_LEFT))
            {
                auto [x, y] = Ares::Input::GetMousePosition();
                auto width = Ares::Application::Get().GetWindow().GetWidth();
                auto height = Ares::Application::Get().GetWindow().GetHeight();

                auto bounds = m_CameraController.GetBounds();
                auto pos = m_CameraController.GetCamera().GetPosition();
                x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
                y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
                m_Particle.Position = { x + pos.x, y + pos.y };
                for (int i = 0; i < 5; i++)
                    m_ParticleSystem.Emit(m_Particle);
            }

            m_ParticleSystem.OnUpdate(deltaTime);
            m_ParticleSystem.OnRender(m_CameraController.GetCamera());*/


        
            /*Ares::Renderer2D::BeginScene(m_CameraController.GetCamera());


            for (uint32_t y = 0; y < m_MapHeight; y++)
            {
                for (uint32_t x = 0; x < m_MapHeight; x++)
                {
                    char tileType = s_MapTiles[x + y * m_MapWidth];

                    Ares::Ref<Ares::SubTexture2D> texture;

                    if (s_TextureMap.find(tileType) != s_TextureMap.end())
                    {
                        texture = s_TextureMap[tileType];
                    }
                    else
                    {
                        texture = m_TextureBarrel;
                    }

                    Ares::Renderer2D::DrawQuad(
                        { x - m_MapWidth * .5f, y - m_MapHeight * .5f }, 0.0f, { 1.0f, 1.0f }, 
                        texture, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f }
                    );
                }

            }*/
            /*Ares::Renderer2D::DrawQuad({ 0.0f, 0.0f }, 0.0f, { 1.0f, 1.0f }, m_TextureStairs, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
            Ares::Renderer2D::DrawQuad({ 1.0f, 0.0f }, 0.0f, { 1.0f, 1.0f }, m_TextureBarrel, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });
            Ares::Renderer2D::DrawQuad({ -1.0f, 0.0f }, 0.0f, { 1.0f, 2.0f }, m_TextureTree, 1.0f, { 1.0f, 1.0f, 1.0f, 1.0f });*/

        
            //Ares::Renderer2D::EndScene();

            //auto stats = Ares::Renderer2D::GetStats();
            // wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
            /*float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); 
            float averageFPS = 1.0f / averageRenderTime;
            char buffer[64];
            glfwSetWindowTitle((GLFWwindow*)Ares::Application::Get().GetWindow().GetNativeWindow(), buffer);*/
        }
    

    }

    void EditorLayer::OnImGuiDraw()
    {

        ARES_PROFILE_FUNCTION();















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
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

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















        ImGui::Begin("Settings");
        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

        auto stats = Ares::Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indicies: %d", stats.GetTotalIndexCount());



        /*ImGui::Text("Textures: %d", stats.TextureCount);
        ImGui::Text("Frame count: %d", stats.FrameCount);*/

        // wont be accurate until we have gathered at least stats.FrameRenderTime().size() results
        //float averageRenderTime = stats.TotalFrameRenderTime / stats.FrameRenderTime.size(); 
        //float averageFPS = 1.0f / averageRenderTime;
        //ImGui::Text("Average frame render time: %8.5f (%5.0f fps)", averageRenderTime, averageFPS);
        //ImGui::End();

        /*for (auto& result : m_ProfileResults)
        {
            char label[50];
            strcpy(label, "%.3fms  ");
            strcat(label, result.Name);
            ImGui::Text(label, result.Time);
        }

        m_ProfileResults.clear();*/

        //uint32_t textureID = m_Texture->GetRendererID();


        ImGui::End();



        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);


        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        /*if (viewportSize.x < 1)
            viewportSize.x = 1;
        if (viewportSize.y < 1)
            viewportSize.y = 1;*/


        /*if (m_ViewportSize != *((glm::vec2*)&viewportSize))
        {

            m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);*/
            m_ViewportSize = { viewportSize.x, viewportSize.y };

            //m_CameraController.OnResize(viewportSize.x, viewportSize.y);

            /*m_FrameBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
            m_ViewportSize = { viewportSize.x, viewportSize.y };*/
        //}


        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, ImVec2{ viewportSize.x, viewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar();

        
        ImGui::End();
    }

    void EditorLayer::OnEvent(Ares::Event& e)
    {
        m_CameraController.OnEvent(e);
    }
}
