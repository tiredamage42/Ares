

#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Ares/Renderer/Renderer.h"

namespace Ares {


	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment")
			return GL_FRAGMENT_SHADER;

		ARES_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		// extract name from filepath `assets/shaders/shader.glsl`
		// find last of forward slash or back slash
		size_t lastSlashI = filepath.find_last_of("/\\");
		lastSlashI = lastSlashI == std::string::npos ? 0 : lastSlashI + 1;
		size_t lastDotI = filepath.rfind('.');
		auto count = lastDotI == std::string::npos ? filepath.size() - lastSlashI : lastDotI - lastSlashI;
		m_Name = filepath.substr(lastSlashI, count);

		
		std::string source = ReadFile(filepath);
		std::unordered_map<GLenum, std::string> sources = PreProcess(source);

		Renderer::Submit([=]() {
			Compile(sources);
		});
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		Renderer::Submit([=]() {
			Compile(sources);
		});
	}

	OpenGLShader::~OpenGLShader()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteProgram(rendererID);
		});
	}

	void OpenGLShader::Bind()
	{
		Renderer::Submit([this]() {
			glUseProgram(this->m_RendererID);
		});
	}

	void OpenGLShader::Unbind() const
	{
		Renderer::Submit([]() {
			glUseProgram(0);
		});
	}




	std::string OpenGLShader::ReadFile(const std::string& filePath) const
	{
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			// go to the end
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				// resize reult to size of file
				result.resize(size);
				// go to the beginning of the files
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				ARES_CORE_ERROR("Could not read from file '{0}'", filePath);
			}
		}
		else
		{
			ARES_CORE_ERROR("Could not open file: {0}", filePath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		// token to split on
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);

		// find the first one
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);

			// if there were no lines after the #type
			ARES_CORE_ASSERT(eol != std::string::npos, "Syntax Error");

			// beginning of the type string (after '#type')
			size_t begin = pos + typeTokenLength + 1;

			// the actual type as string
			std::string type = source.substr(begin, eol - begin);
			ARES_CORE_ASSERT(ShaderTypeFromString(type), "Invalid Shader Type Specified");

			// get next line
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			ARES_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");

			// find the next #type line
			pos = source.find(typeToken, nextLinePos);

			// source shader code is from the next line, until either
			// the end of the file, or the next line we found #type
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		// Get a program object.
		GLuint program = glCreateProgram();

		ARES_CORE_ASSERT(shaderSources.size() <= 2, "Max 2 Shaders In Source");
		std::array<GLenum, 2> glShaderIDs;

		int i = 0;
		// Read our shaders into the appropriate buffers
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			// Create an empty shader handle
			GLuint shader = glCreateShader(type);

			// Send the shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the shader
			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				// get error logs
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				ARES_CORE_ERROR("{0}", infoLog.data());
				ARES_CORE_ASSERT(false, "Shader Compilation Failure!");
				break;
			}

			// Attach our shaders to our program
			glAttachShader(program, shader);

			glShaderIDs[i++] = shader;
		}


		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			// get error logs
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			// Don't leak shaders either.
			for (auto id : glShaderIDs)
				glDeleteShader(id);


			// Use the infoLog as you see fit.

			ARES_CORE_ERROR("{0}", infoLog.data());
			ARES_CORE_ASSERT(false, "Shader Link Failure!");
			return;
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
		{

			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;

	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		Renderer::Submit([=]() {
			UploadUniformInt(name, value);
		});
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, const uint32_t count, bool deleteFromMem)
	{
		Renderer::Submit([=]() mutable {	
			UploadUniformIntArray(name, values, count);
			if (deleteFromMem)
				delete[] values;
			/*{
			}*/
		});
	}
	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat(name, value);
		});
	}
	void OpenGLShader::SetFloat2(const std::string& name, glm::vec2 value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat2(name, value);
		});
	}
	void OpenGLShader::SetFloat3(const std::string& name, glm::vec3 value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat3(name, value);
		});
	}
	void OpenGLShader::SetFloat4(const std::string& name, glm::vec4 value)
	{
		Renderer::Submit([=]() {
			UploadUniformFloat4(name, value);
		});
	}
	void OpenGLShader::SetMat3(const std::string& name, glm::mat3 value)
	{
		Renderer::Submit([=]() {
			UploadUniformMat3(name, value);
		});
	}
	void OpenGLShader::SetMat4(const std::string& name, glm::mat4 value)
	{
		Renderer::Submit([=]() {
			UploadUniformMat4(name, value);
		});
	}

	
	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, glm::vec2 value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, glm::vec3 value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, glm::vec4 value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, glm::mat3 value)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, glm::mat4 value)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	GLint OpenGLShader::GetUniformLocation(const std::string& name) const
	{

		if (m_UniformLocationMap.find(name) != m_UniformLocationMap.end())
			return m_UniformLocationMap[name];
		
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1)
		{
			ARES_CORE_WARN("Uniform '{0}' not found in shader '{1}'!", name, m_Name);
			return location;
		}
		m_UniformLocationMap[name] = location;
		return location;
	}
}