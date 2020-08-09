#pragma once

#include "Ares/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShaderUniform.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

// TODO: remove
typedef unsigned int GLenum;

namespace Ares {
	class OpenGLShader : public Shader
	{
	public:
		//enum class ShaderType
		//{
		//	None = 0,
		//	Vertex = 1,

		//	// Fragment and Pixel shaders are the same
		//	Fragment = 2,
		//	Pixel = 2
		//};

		OpenGLShader() = default;
		virtual ~OpenGLShader();

		OpenGLShader(const std::string& filePath);
		//OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

		virtual void Reload() override;

		virtual void Bind() override;
		virtual void Unbind() const override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; }


		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count, bool deleteFromMem = true) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat2(const std::string& name, glm::vec2 value) override;
		virtual void SetFloat3(const std::string& name, glm::vec3 value) override;
		virtual void SetFloat4(const std::string& name, glm::vec4 value) override;
		virtual void SetMat3(const std::string& name, glm::mat3 value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetIntFromRenderThread(const std::string& name, int value) override;
		virtual void SetIntArrayFromRenderThread(const std::string& name, int* values, uint32_t count, bool deleteFromMem = true) override;
		virtual void SetFloatFromRenderThread(const std::string& name, float value) override;
		virtual void SetFloat2FromRenderThread(const std::string& name, glm::vec2 value) override;
		virtual void SetFloat3FromRenderThread(const std::string& name, glm::vec3 value) override;
		virtual void SetFloat4FromRenderThread(const std::string& name, glm::vec4 value) override;
		virtual void SetMat3FromRenderThread(const std::string& name, glm::mat3 value) override;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value) override;

		virtual void SetVSMaterialUniformBuffer(Buffer buffer) override;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer) override;

		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;

		

		//virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

	protected:
		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, glm::vec2 value);
		void UploadUniformFloat3(const std::string& name, glm::vec3 value);
		void UploadUniformFloat4(const std::string& name, glm::vec4 value);
		void UploadUniformMat3(const std::string& name, glm::mat3 value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);


		void UploadUniformInt(uint32_t location, int value);
		void UploadUniformIntArray(uint32_t location, int* values, uint32_t count);
		void UploadUniformFloat(uint32_t location, float value);
		void UploadUniformFloat2(uint32_t location, glm::vec2 value);
		void UploadUniformFloat3(uint32_t location, glm::vec3 value);
		void UploadUniformFloat4(uint32_t location, glm::vec4 value);
		void UploadUniformMat3(uint32_t location, glm::mat3 value);
		void UploadUniformMat4(uint32_t location, const glm::mat4& value);
		void UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count);

		void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset);

	private:

		inline const ShaderUniformBufferList& GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
		inline const ShaderUniformBufferList& GetPSRendererUniforms() const override { return m_PSRendererUniformBuffers; }
		
		virtual bool HasVSMaterialUniformBuffer() const override { return (bool)m_VSMaterialUniformBuffer; }
		virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }

		inline const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
		inline const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
		inline const ShaderResourceList& GetResources() const override { return m_Resources; }

		


		//static ShaderType ShaderTypeFromString(const std::string& type);

		std::string ReadShaderFromFile(const std::string& filePath) const;
		//void ReadShaderFromFile(const std::string& filePath);
		void CompileAndUploadShader();



		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Parse();
		void ParseUniform(const std::string& statement, ShaderDomain domain);
		void ParseUniformStruct(const std::string& block, ShaderDomain domain);
		ShaderStruct* FindStruct(const std::string& name);

		void ResolveUniforms();
		void ValidateUniforms();

		void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer);
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
		void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer);
		void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset);

		//std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		int32_t GetUniformLocation(const std::string& name) const;
		
		//void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		//mutable std::unordered_map<std::string, GLint> m_UniformLocationMap;

		// unique id in opengl
		uint32_t m_RendererID = 0;
		std::string m_Name, m_AssetPath;
		//std::string m_ShaderSource;
		bool m_IsCompute = false;

		bool m_Loaded = false;

		std::unordered_map<GLenum, std::string> m_ShaderSource;

		std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;

		ShaderUniformBufferList m_VSRendererUniformBuffers;
		ShaderUniformBufferList m_PSRendererUniformBuffers;
		Ref<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
		Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;
		ShaderResourceList m_Resources;
		ShaderStructList m_Structs;

	};
}