#include "AresPCH.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include <glad/glad.h>
namespace Ares {

	void OpenGLMessageCallback(
		unsigned source, unsigned type, unsigned id, unsigned severity,
		int length, const char* message, const void* userParam
	)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         
			ARES_CORE_CRITICAL("[OpenGL Debug HIGH] {0}", message);
			ARES_CORE_ASSERT(false, "");
			return;
		case GL_DEBUG_SEVERITY_MEDIUM:       
			ARES_CORE_ERROR("[OpenGL Debug MEDIUM] {0}", message);
			ARES_CORE_ASSERT(false, "");
			return;
		case GL_DEBUG_SEVERITY_LOW:          
			ARES_CORE_WARN("[OpenGL Debug LOW] {0}", message);
			ARES_CORE_ASSERT(false, "");
			return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: 
			ARES_CORE_LOG("[OpenGL Debug NOTIFICATION] {0}", message);
			return;
		}

		ARES_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		// get open gl notifications and higher
#ifdef ARES_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

#ifdef ARES_ENABLE_ASSERTS
		int versionMajor, versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		ARES_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Ares requires at least OpenGL version 4.5!");
#endif
		
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glFrontFace(GL_CCW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_MULTISAMPLE);
		glEnable(GL_STENCIL_TEST);


		auto& caps = RendererAPI::GetCapabilities();

		caps.Vendor = (const char*)glGetString(GL_VENDOR);
		caps.Renderer = (const char*)glGetString(GL_RENDERER);
		caps.Version = (const char*)glGetString(GL_VERSION);

		glGetIntegerv(GL_MAX_SAMPLES, &caps.MaxSamples);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &caps.MaxAnisotropy);

		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &caps.MaxTextureUnits);

		GLenum error = glGetError();
		while (error != GL_NO_ERROR)
		{
			ARES_CORE_ERROR("OpenGL Error {0}", error);
			error = glGetError();
		}
	}
	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	void OpenGLRendererAPI::Clear(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	
	void OpenGLRendererAPI::DrawIndexed(uint32_t indexCount, PrimitiveType type, bool depthTest)
	{
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);

		GLenum glPrimitiveType = 0;
		switch (type)
		{
		case PrimitiveType::Triangles:
			glPrimitiveType = GL_TRIANGLES;
			break;
		case PrimitiveType::Lines:
			glPrimitiveType = GL_LINES;
			break;
		}

		glDrawElements(glPrimitiveType, indexCount, GL_UNSIGNED_INT, nullptr);
		

		if (!depthTest)
			glEnable(GL_DEPTH_TEST);
	}
	void OpenGLRendererAPI::SetLineThickness(float thickness)
	{
		glLineWidth(thickness);
	}

}