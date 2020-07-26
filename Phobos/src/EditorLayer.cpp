#include "EditorLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ares
{

    EditorLayer::EditorLayer()
        : Layer("Sandbox2D"), 
        m_CameraController(1280.0f / 720.0f)
    {
    }
    void EditorLayer::OnAttach()
    {
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
    }

    void EditorLayer::OnDetach()
    {
        
    }
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
        FrameBufferSpecs spec = m_FrameBuffer->GetSpecs();
        // zero sized framebuffer is invalid
        if (m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y))
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        
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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportSize.x, viewportSize.y };

        uint32_t textureID = m_FrameBuffer->GetColorAttachmentRendererID();
        ImGui::Image((void*)textureID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();

    }

    void EditorLayer::OnEvent(Ares::Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}
