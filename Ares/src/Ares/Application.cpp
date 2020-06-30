#include "Application.h"
#include "Ares/Log.h"
#include "Ares/Events/ApplicationEvent.h"

namespace Ares {
    Application::Application() {

    }
    Application::~Application() {

    }

    void Application::Run() {

        WindowResizeEvent e(1200, 720);
        ARES_CORE_LOG(e);
        while (true) {}
    }
}