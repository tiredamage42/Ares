
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLContext.h"

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
		ARES_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ARES_CORE_ASSERT(status, "Could not initialize Glad!");

		ARES_CORE_INFO("OpenGL Info:");
		ARES_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		ARES_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		ARES_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));


#ifdef ARES_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		ARES_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Ares requires at least OpenGL version 4.5!");
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		ARES_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}