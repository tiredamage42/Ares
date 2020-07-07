#pragma once

#include "AresPCH.h"

#include "Core.h"
#include "Events/Event.h"

#include "Ares/Events/ApplicationEvent.h"

#include "Window.h "
namespace Ares {

    class ARES_API Application
    {
    public:
        Application();
        virtual ~Application();
        void Run();

        void OnEvent(Event& e);
    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    };

    // defined in client
    Application* CreateApplication();
}
