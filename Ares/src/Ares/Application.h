#pragma once
#include "Core.h"
#include "Events/Event.h"

namespace Ares {

    class ARES_API Application
    {
    public:
        Application();
        virtual ~Application();
        void Run();
    };

    // defined in client
    Application* CreateApplication();
}
