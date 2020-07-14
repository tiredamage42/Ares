#pragma once

#include "AresPCH.h"
#include "Core.h"
#include "Ares/Events/Event.h"
#include "Ares/Events/ApplicationEvent.h"
#include "Window.h "
#include "Ares/Core/LayerStack.h"

#include "Ares/Core/Time.h"

#include "Ares/ImGui/ImGuiLayer.h"


namespace Ares {

    class Application
    {
    public:
        Application();
        virtual ~Application() = default;
        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline Window& GetWindow() { return *m_Window; };
        
        inline static Application& Get() { return *s_Instance; }
    
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        std::unique_ptr<Window> m_Window;

        ImGuiLayer* m_ImGuiLayer;

        bool m_Running = true;
        bool m_Minimized = false;

        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;

        static Application* s_Instance;
    };

    // defined in client
    Application* CreateApplication();
}