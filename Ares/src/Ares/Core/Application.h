#pragma once

#include "AresPCH.h"
#include "Ares/Core/Core.h"
#include "Ares/Events/Event.h"
#include "Ares/Events/ApplicationEvent.h"
#include "Ares/Core/Window.h "
#include "Ares/Core/LayerStack.h"
#include "Ares/Core/Time.h"

#include "Ares/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Ares {

    class Application
    {
    public:
        Application(const WindowProps& props);
        virtual ~Application();
        
        void OnEvent(Event& e);

        void Close();

        ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }


        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline Window& GetWindow() { return *m_Window; };
        
        inline static Application& Get() { return *s_Instance; }
    
    private:
        void Run();

        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);

        std::unique_ptr<Window> m_Window;

        ImGuiLayer* m_ImGuiLayer;

        bool m_Running = true;
        bool m_Minimized = false;

        LayerStack m_LayerStack;

        float m_LastFrameTime = 0.0f;

        static Application* s_Instance;

        friend int ::main(int argc, char** argv);
    };

    // defined in client
    Application* CreateApplication();
}
