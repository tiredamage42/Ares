#include "AresPCH.h"
#include "Ares/Core/Application.h"
#include "Ares/Core/Log.h"
#include "Ares/Core/Input.h"
#include "Ares/Core/Time.h"
#include "Ares/Core/Platform.h"

#include "Ares/Renderer/Renderer.h"
#include "Ares/Renderer/Framebuffer.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


namespace Ares {

    Application* Application::s_Instance = nullptr;

    Application::Application(const WindowProps& props)
    {

        ARES_CORE_ASSERT(!s_Instance, "Application Instance Already Exists!");
        s_Instance = this;

        m_Window = Window::Create(props);
        m_Window->SetEventCallback(ARES_BIND_EVENT_FN(Application::OnEvent));
        m_Window->SetVSync(false);

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        Renderer::Init();
        Renderer::WaitAndRender();

        Input::InitializeInput();
    }

    Application::~Application()
    {
        Renderer::Shutdown();
    }
    
    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        // if dispatcher sees window close event, dispatch it to
        // OnWindowClose / OnWindowResize
        dispatcher.Dispatch<WindowCloseEvent>(ARES_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ARES_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            (*it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
    }
    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
    void Application::Close()
    {
        m_Running = false;
    }

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;

        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());


        auto& fbs = FramebufferPool::GetGlobal()->GetAll();
        for (auto& fb : fbs)
        {
            fb->Resize(e.GetWidth(), e.GetHeight());
        }

        return false;
    }

    void Application::RenderImGui()
    {
        // draw custom imgui for debugging, etc...
        m_ImGuiLayer->BeginImGui();

        for (Layer* layer : m_LayerStack)
            layer->OnImGuiDraw();

        m_ImGuiLayer->EndImGui();
    }

    void Application::Run() 
    {
        while (m_Running)
        {        
            ARES_PROFILE_SCOPE("Update Loop");

            Time::Tick(Platform::GetTime()); // Platform::GetTime

            Input::UpdateInputPolls();

            if (!m_Minimized)
            {
                {
                    ARES_PROFILE_SCOPE("Layer Updates");
                    for (Layer* layer : m_LayerStack)
                    {
                        layer->OnUpdate();
                    }
                }

                // Render ImGui on render thread
                {    
                    ARES_PROFILE_SCOPE("Render imgui");
                    Renderer::Submit([=]() { RenderImGui(); }, "RenderIMGUI");
                }
                {
                    ARES_PROFILE_SCOPE("Wait and render");
                    Renderer::WaitAndRender();
                }
            
            }

            {
                ARES_PROFILE_SCOPE("Window update");

                m_Window->OnUpdate();
            }
        }
    }

    std::string Application::OpenFile(const char* filter) const
    {
        OPENFILENAMEA ofn;       // common dialog box structure
        CHAR szFile[260] = { 0 };       // if using TCHAR macros

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)m_Window->GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return std::string();
    }
    std::string Application::SaveFile(const char* filter) const
    {
        OPENFILENAMEA ofn;              // common dialog box structure
        CHAR szFile[260] = { 0 };       // if using TCHAR macros

        // Initialize OPENFILENAME
        ZeroMemory(&ofn, sizeof(OPENFILENAME));
        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)m_Window->GetNativeWindow());
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return std::string();
    }



    const char* Application::GetConfigurationName()
    {
#if defined(ARES_DEBUG)
        return "Debug";
#elif defined(ARES_RELEASE)
        return "Release";
#elif defined(ARES_DIST)
        return "Dist";
#else
#error Undefined configuration?
#endif
    }


    const char* Application::GetPlatformName()
    {
        return Platform::GetName();
    }

}