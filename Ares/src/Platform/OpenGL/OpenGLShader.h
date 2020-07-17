#pragma once

//#include <string>
#include "Ares/Renderer/Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

// TODO: remove
typedef unsigned int GLenum;

namespace Ares {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		virtual ~OpenGLShader();
		
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;

		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, glm::vec2 value) override;
		virtual void SetFloat3(const std::string& name, glm::vec3 value) override;
		virtual void SetFloat4(const std::string& name, glm::vec4 value) override;
		virtual void SetMat3(const std::string& name, glm::mat3 value) override;
		virtual void SetMat4(const std::string& name, glm::mat4 value) override;

	protected:
		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, glm::vec2 value);
		void UploadUniformFloat3(const std::string& name, glm::vec3 value);
		void UploadUniformFloat4(const std::string& name, glm::vec4 value);

		void UploadUniformMat3(const std::string& name, glm::mat3 value);
		void UploadUniformMat4(const std::string& name, glm::mat4 value);

	private:

		GLint GetUniformLocation(const std::string& name) const;

		std::string ReadFile(const std::string& filePath);

		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);


		mutable std::unordered_map<std::string, GLint> m_UniformLocationMap;


		// unique id in opengl
		uint32_t m_RendererID;


		std::string m_Name;

	};
}