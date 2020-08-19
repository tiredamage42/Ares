
#include "AresPCH.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace Ares {
	std::vector<Ref<Shader>> Shader::s_AllShaders;
	std::unordered_map<std::string, Ref<Shader>> Shader::s_ShaderMap;

	Ref<Shader> Shader::Create(const std::string& filePath)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			Ref<Shader> result = CreateRef<OpenGLShader>(filePath);
			s_AllShaders.push_back(result);
			return result;
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Find(const std::string& filePath)
	{

		if (s_ShaderMap.find(filePath) != s_ShaderMap.end())
			return s_ShaderMap.at(filePath);
		
		Ref<Shader> shader = Create(filePath);

		s_ShaderMap[filePath] = shader;

		return shader;
	}
}