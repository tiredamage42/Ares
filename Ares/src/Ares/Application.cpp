#include "AresPCH.h"

#include "Application.h"
#include "Ares/Events/ApplicationEvent.h"
#include "Ares/Log.h"

#include <GLFW/glfw3.h>

namespace Ares {
    Application::Application() 
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
    }

    Application::~Application() 
    {

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