#include "AresPCH.h"
#include "Platform/Windows/WindowsWindow.h"

#include "Ares/Core/Input.h"

#include "Ares/Events/ApplicationEvent.h"
#include "Ares/Events/MouseEvent.h"
#include "Ares/Events/KeyEvent.h"

#include "Ares/Renderer/Renderer.h"

#include <glad/glad.h>

//#include "Platform/OpenGL/OpenGLContext.h"

namespace Ares {

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description) {
		ARES_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		ARES_PROFILE_FUNCTION();

		Init(props);
	}
	WindowsWindow::~WindowsWindow()
	{
		ARES_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		ARES_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		ARES_CORE_LOG("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		// only initialize glfw once (might have several window instances)
		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			ARES_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			
#if defined(ARES_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			s_GLFWWindowCount++;
		}

		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ARES_CORE_ASSERT(status, "Could not initialize Glad!");


		/*m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();*/
		
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// set glfw callbacks with lambdas:
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action) {
				case GLFW_PRESS:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(static_cast<KeyCode>(key), 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(static_cast<KeyCode>(keycode));
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action) {
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(static_cast<MouseButtonCode>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(static_cast<MouseButtonCode>(button));
					data.EventCallback(event);
					break;
				}
			}
		});
		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);	
		});
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		ARES_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);

		s_GLFWWindowCount--;
		if (s_GLFWWindowCount == 0)
		{
			ARES_CORE_INFO("Terminating GLFW");
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		ARES_PROFILE_FUNCTION();

		glfwPollEvents();

		glfwSwapBuffers(m_Window);
		//m_Context->SwapBuffers();
	}
	void WindowsWindow::SetVSync(bool enabled)
	{
		ARES_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}