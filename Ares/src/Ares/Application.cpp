#include "AresPCH.h"

#include "Application.h"
//#include "Ares/Events/ApplicationEvent.h"
#include "Ares/Log.h"

#include <GLFW/glfw3.h>

namespace Ares {


#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

    Application::Application() 
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
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

        ARES_CORE_LOG("{0}", e);
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
            m_Window->OnUpdate();
        }
    }
}