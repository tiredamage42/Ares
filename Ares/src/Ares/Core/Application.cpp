#include "AresPCH.h"
#include "Application.h"
#include "Ares/Core/Log.h"
#include "Ares/Renderer/Renderer.h"
#include "Input.h"
#include <GLFW/glfw3.h>
namespace Ares {

    Application* Application::s_Instance = nullptr;

    Application::Application() 
        
    {
        ARES_CORE_ASSERT(!s_Instance, "Application Instance Already Exists!");
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(ARES_BIND_EVENT_FN(Application::OnEvent));
        //m_Window->SetVSync(false);

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer();

        PushOverlay(m_ImGuiLayer);
    }
    
    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        // if dispatcher sees window close event, dispatch it to
        // OnWindowClose
        dispatcher.Dispatch<WindowCloseEvent>(ARES_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ARES_BIND_EVENT_FN(Application::OnWindowResize));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
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

    bool Application::OnWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 || e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }
        m_Minimized = false;

        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

        return false;
    }

    void Application::Run() 
    {
        while (m_Running)
        {        
            float time = (float)glfwGetTime(); // Platform::GetTime
            
            float deltaTime = time - m_LastFrameTime;

            m_LastFrameTime = time;

            if (!m_Minimized)
            {
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(deltaTime);
            }

            // draw custom imgui for debugging, etc...
            m_ImGuiLayer->BeginImGui();        
            
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiDraw();
            
            m_ImGuiLayer->EndImGui();

            m_Window->OnUpdate();
        }
    }
}
