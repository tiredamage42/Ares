#include "AresPCH.h"
#include "Application.h"
#include "Ares/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Ares {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application() 
    {
        ARES_CORE_ASSERT(!s_Instance, "Application Instance Already Exists!");
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_ImGuiLayer = new ImGuiLayer();

        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application() 
    {
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        // if dispatcher sees window close event, dispatch it to
        // OnWindowClose
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

        //ARES_CORE_LOG("{0}", e);
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
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }

    void Application::Run() 
    {
        while (m_Running)
        {
            glClearColor(1, .5, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            // draw custom imgui for debugging, etc...
            m_ImGuiLayer->BeginImGui();        
            
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiDraw();
            
            m_ImGuiLayer->EndImGui();

            //auto [x, y] = Input::GetMousePosition();
            //ARES_CORE_LOG("{0}, {1}", x, y);

            m_Window->OnUpdate();
        }
    }
}