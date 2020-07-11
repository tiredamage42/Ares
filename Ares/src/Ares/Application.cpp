#include "AresPCH.h"
#include "Application.h"
#include "Ares/Log.h"

#include "Ares/Renderer/Renderer.h"

#include "Input.h"

namespace Ares {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;



    Application::Application() 
    {
        ARES_CORE_ASSERT(!s_Instance, "Application Instance Already Exists!");
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_ImGuiLayer = new ImGuiLayer();

        PushOverlay(m_ImGuiLayer);


        m_VertexArray.reset(VertexArray::Create());

        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
             0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        m_VertexBuffer.reset( VertexBuffer::Create(vertices, sizeof(vertices)) );
        m_VertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" }
        });
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);

        // index buffer specifies order to draw vertices
        uint32_t indicies[3] = { 0,1,2 };
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        m_IndexBuffer.reset(IndexBuffer::Create(indicies, sizeof(indicies) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        
        m_SquareVA.reset(VertexArray::Create());

        float squareVertices[3 * 4] = {
            -0.75f, -0.75f, 0.0f,
             0.75f, -0.75f, 0.0f,
             0.75f,  0.75f, 0.0f,
            -0.75f,  0.75f, 0.0f
        };

        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
        });
        m_SquareVA->AddVertexBuffer(squareVB);

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        std::string vertexSrc = R"(
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

out vec3 v_Position;
out vec4 v_Color;
            
void main()
{
    gl_Position = vec4(a_Position, 1);
    v_Position = a_Position;
    v_Color = a_Color;
}

)";
        std::string fragmentSrc = R"(
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Color;
            
void main()
{
    color = vec4(v_Position * 0.5 + 0.5, 1.0);
    color = v_Color;
}

)";


        m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

        std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

        std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

        m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
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

        //ARES_CORE_LOG("{0}", e);
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
        {
            (*--it)->OnEvent(e);
            if (e.Handled)
                break;
        }
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* layer)
    {
        m_LayerStack.PushOverlay(layer);
        layer->OnAttach();
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
            RenderCommand::SetClearColor({ 1, .5, 0, 1 });
            RenderCommand::Clear();

            // submission
            Renderer::BeginScene();

            // submit an actual mesh, geometry, etc.
            m_BlueShader->Bind();
            Renderer::Submit(m_SquareVA);

            m_Shader->Bind();
            Renderer::Submit(m_VertexArray);


            Renderer::EndScene();
            
            m_Shader->Unbind();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            // draw custom imgui for debugging, etc...
            m_ImGuiLayer->BeginImGui();        
            
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiDraw();
            
            m_ImGuiLayer->EndImGui();

            //auto [x, y] = Input::GetMousePosition();
            //ARES_CORE_LOG("{0}, {1}", x, y);

            m_Window->OnUpdate();
        }
    }
}



