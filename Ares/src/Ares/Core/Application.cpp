#include "AresPCH.h"
#include "Ares/Core/Application.h"
#include "Ares/Core/Log.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Core/Input.h"

#include "Ares/Core/Time.h"
#include <GLFW/glfw3.h>
namespace Ares {

    Application* Application::s_Instance = nullptr;

    Application::Application(const WindowProps& props)
    {
        ARES_PROFILE_FUNCTION();

        ARES_CORE_ASSERT(!s_Instance, "Application Instance Already Exists!");
        s_Instance = this;


        m_Window = Window::Create(props);
        m_Window->SetEventCallback(ARES_BIND_EVENT_FN(Application::OnEvent));
        //m_Window->SetVSync(false);

        Renderer::Init();

        m_ImGuiLayer = new ImGuiLayer();

        PushOverlay(m_ImGuiLayer);
    }
    Application::~Application()
    {
        ARES_PROFILE_FUNCTION();

        Renderer::Shutdown();
    }
    
    void Application::OnEvent(Event& e)
    {
        ARES_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);

        // if dispatcher sees window close event, dispatch it to
        // OnWindowClose
        dispatcher.Dispatch<WindowCloseEvent>(ARES_BIND_EVENT_FN(Application::OnWindowClose));
        dispatcher.Dispatch<WindowResizeEvent>(ARES_BIND_EVENT_FN(Application::OnWindowResize));

        //for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            //(*--it)->OnEvent(e);
            (*it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    
    void Application::PushLayer(Layer* layer)
    {
        ARES_PROFILE_FUNCTION();

        m_LayerStack.PushLayer(layer);
    }

    void Application::PushOverlay(Layer* layer)
    {
        ARES_PROFILE_FUNCTION();

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
        ARES_PROFILE_FUNCTION();

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
        ARES_PROFILE_FUNCTION();

        while (m_Running)
        {        
            ARES_PROFILE_SCOPE("RunLoop");

            Time::Tick(glfwGetTime()); // Platform::GetTime

            /*float time = (float)glfwGetTime(); 
            
            float deltaTime = time - m_LastFrameTime;

            float ms = deltaTime * 1000.0f;

            m_LastFrameTime = time;*/

            if (!m_Minimized)
            {
                {
                    ARES_PROFILE_SCOPE("Layerstack OnUpdate");

                    for (Layer* layer : m_LayerStack)
                        layer->OnUpdate();// deltaTime);
                }
            }

            // draw custom imgui for debugging, etc...
            m_ImGuiLayer->BeginImGui();        
            
            {
                ARES_PROFILE_SCOPE("Layerstack OnImGuiDraw");

                for (Layer* layer : m_LayerStack)
                    layer->OnImGuiDraw();
            }
            
            m_ImGuiLayer->EndImGui();

            m_Window->OnUpdate();


            // TODO:  profiler frame++
        }
    }
}
