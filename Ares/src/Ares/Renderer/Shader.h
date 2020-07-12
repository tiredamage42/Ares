#pragma once

#include <string>
#include <glm/glm.hpp>
namespace Ares {
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();
		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string& name, glm::mat4 matrix);
	private:

		// unique id in opengl
		uint32_t m_RendererID;

	};
}