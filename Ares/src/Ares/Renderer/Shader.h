#pragma once

#include <string>

namespace Ares {
	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();
		void Bind() const;
		void Unbind() const;
	private:

		// unique id in opengl
		uint32_t m_RendererID;

	};
}