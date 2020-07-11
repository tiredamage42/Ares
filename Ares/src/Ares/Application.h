#pragma once

#include "AresPCH.h"
#include "Core.h"
#include "Events/Event.h"
#include "Ares/Events/ApplicationEvent.h"
#include "Window.h "
#include "Ares/LayerStack.h"
#include "Ares/ImGui/ImGuiLayer.h"

#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/Buffer.h"
#include "Ares/Renderer/VertexArray.h"

namespace Ares {

    class ARES_API Application
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

        std::unique_ptr<Window> m_Window;

        ImGuiLayer* m_ImGuiLayer;

        bool m_Running = true;

        LayerStack m_LayerStack;


        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;

        std::shared_ptr<Shader> m_BlueShader;
        std::shared_ptr<VertexArray> m_SquareVA;

        static Application* s_Instance;
    };

    // defined in client
    Application* CreateApplication();
}
