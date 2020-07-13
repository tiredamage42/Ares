#pragma once

//#include <string>
#include "Ares/Renderer/Shader.h"
//#include <glad/glad.h>
#include <glm/glm.hpp>

// TODO: remove
typedef unsigned int GLenum;

namespace Ares {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& filePath);

		virtual ~OpenGLShader();
		
		virtual void Bind() const override;
		virtual void Unbind() const override;

		void UploadUniformInt(const std::string& name, int value);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, glm::vec2 value);
		void UploadUniformFloat3(const std::string& name, glm::vec3 value);
		void UploadUniformFloat4(const std::string& name, glm::vec4 value);

		void UploadUniformMat3(const std::string& name, glm::mat3 value);
		void UploadUniformMat4(const std::string& name, glm::mat4 value);

	private:

		std::string ReadFile(const std::string& filePath);

		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		// unique id in opengl
		uint32_t m_RendererID;

	};
}