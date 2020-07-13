#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/glm.hpp>

#include "Ares/Renderer/Shader.h"
namespace Ares {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
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

		// unique id in opengl
		uint32_t m_RendererID;

	};
}