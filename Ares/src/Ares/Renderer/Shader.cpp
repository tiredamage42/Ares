
#include "AresPCH.h"
#include "Shader.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace Ares {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertexSrc, fragmentSrc);
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
	Shader* Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return new OpenGLShader(filePath);
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
}