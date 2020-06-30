#pragma once
#include "Core.h"

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
