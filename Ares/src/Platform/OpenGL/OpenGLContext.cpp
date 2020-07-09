
#include "AresPCH.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Ares {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ARES_CORE_ASSERT(windowHandle, "OpenGL window handle is null!");
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ARES_CORE_ASSERT(status, "Could not initialize Glad!");

		ARES_CORE_INFO("OpenGL Info:");
		ARES_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		ARES_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		ARES_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}