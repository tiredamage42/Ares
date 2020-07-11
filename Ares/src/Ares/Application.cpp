#include "AresPCH.h"
#include "Application.h"
#include "Ares/Log.h"

#include <glad/glad.h>

#include "Input.h"

namespace Ares {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;


    static GLenum ShaderDataType2OpenGLBaseType(ShaderDataType type) {
        switch (type)
        {
        case Ares::ShaderDataType::Float:	return GL_FLOAT;
        case Ares::ShaderDataType::Float2:	return GL_FLOAT;
        case Ares::ShaderDataType::Float3:	return GL_FLOAT;
        case Ares::ShaderDataType::Float4:	return GL_FLOAT;
        case Ares::ShaderDataType::Mat3:	return GL_FLOAT;
        case Ares::ShaderDataType::Mat4:	return GL_FLOAT;
        case Ares::ShaderDataType::Int:		return GL_INT;
        case Ares::ShaderDataType::Int2:	return GL_INT;
        case Ares::ShaderDataType::Int3:	return GL_INT;
        case Ares::ShaderDataType::Int4:	return GL_INT;
        case Ares::ShaderDataType::Bool:	return GL_BOOL;
        }
        ARES_CORE_ASSERT(false, "Unknown ShaderDataType");
        return 0;
    }


    Application::Application() 
    {
        ARES_CORE_ASSERT(!s_Instance, "Application Instance Already Exists!");
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

        m_ImGuiLayer = new ImGuiLayer();

        PushOverlay(m_ImGuiLayer);


        // 1 vertex array
        glGenVertexArrays(1, &m_VertexArray);
        glBindVertexArray(m_VertexArray);


        
        float vertices[3 * 7] = {
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
             0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f
        };

        m_VertexBuffer.reset( VertexBuffer::Create(vertices, sizeof(vertices)) );
        //m_VertexBuffer->Bind();

        {

            BufferLayout layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float4, "a_Color" }
            };

            m_VertexBuffer->SetLayout(layout);
        }


        
        const auto& layout = m_VertexBuffer->GetLayout();

        uint32_t index = 0;
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index, 
                element.GetComponentCount(), 
                ShaderDataType2OpenGLBaseType(element.Type), 
                element.Normalized ? GL_TRUE : GL_FALSE, 
                layout.GetStride(), 
                (const void*)element.Offset
            );

            index++;
        }
        
        // tell open gl layout
        //glEnableVertexAttribArray(0);
        // index, 
        // num attributes, 
        // type, 
        // normalized, 
        // amount of bytes between vertices, 
        // offset
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        uint32_t indicies[3] = { 0,1,2 };

        // index buffer specifies order to draw vertices
        m_IndexBuffer.reset(IndexBuffer::Create(indicies, sizeof(indicies) / sizeof(uint32_t)));

        //glGenBuffers(1, &m_IndexBuffer);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

        ////unsigned int indicies[3] = { 0,1,2 };
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

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
            glClearColor(1, .5, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            m_Shader->Bind();

            glBindVertexArray(m_VertexArray);
            glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

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