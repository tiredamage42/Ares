
#include "AresPCH.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"
namespace Ares {
	std::vector<Ref<Shader>> Shader::s_AllShaders;

	/*Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			Ref<Shader> result = CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
			s_AllShaders.push_back(result);
			return result;
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}*/
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


	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		// make sure map doesnt include name already
		ARES_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}
	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		ARES_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}
	bool ShaderLibrary::Exists(const std::string& name)
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
}