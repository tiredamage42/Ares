#pragma once

#include "Ares/Renderer/Shader.h"
#include "Platform/OpenGL/OpenGLShaderUniform.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Ares {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader() = default;
		virtual ~OpenGLShader();
		OpenGLShader(const std::string& filePath);
		
		virtual void Reload() override;
		virtual void Bind(ShaderVariations variation) override;
		virtual void Unbind() const override;

		virtual uint32_t GetRendererID(ShaderVariations variant) const override;

		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetInt(const std::string& name, const int& value) override;
		virtual void SetFloat(const std::string& name, const float& value) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat3(const std::string& name, const glm::mat3& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
		
		virtual void SetIntArray(const std::string& name, const int32_t* values, uint32_t count) override;
		virtual void SetFloatArray(const std::string& name, const float* values, uint32_t count) override;
		virtual void SetFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count) override;
		virtual void SetFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count) override;
		virtual void SetFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count) override;
		virtual void SetMat3Array(const std::string& name, const glm::mat3& values, uint32_t count) override;
		virtual void SetMat4Array(const std::string& name, const glm::mat4& values, uint32_t count) override;

		virtual void SetPSMaterialUniformBuffer(Buffer buffer) override;
		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;
		virtual const bool IsCurrentlyBound() const override;
		virtual const bool IsLit() const override {return m_IsLit; }

		virtual const bool HasVariation(ShaderVariations variation) const override
		{
			return (uint32_t)variation & m_VariationFlags;
		}

	private:

		const uint32_t CheckBoundAndGetVariationIDX() const;

		void UploadUniformInt	(const std::string& name, const int& value,			uint32_t variationIDX);
		void UploadUniformFloat	(const std::string& name, const float& value,		uint32_t variationIDX);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value,	uint32_t variationIDX);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value,	uint32_t variationIDX);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value,	uint32_t variationIDX);
		void UploadUniformMat3	(const std::string& name, const glm::mat3& value,	uint32_t variationIDX);
		void UploadUniformMat4	(const std::string& name, const glm::mat4& value,	uint32_t variationIDX);
		void UploadUniformIntArray		(const std::string& name, const int32_t* values,	uint32_t count, uint32_t variationIDX);
		void UploadUniformFloatArray	(const std::string& name, const float* values,		uint32_t count, uint32_t variationIDX);
		void UploadUniformFloat2Array	(const std::string& name, const glm::vec2& values,	uint32_t count, uint32_t variationIDX);
		void UploadUniformFloat3Array	(const std::string& name, const glm::vec3& values,	uint32_t count, uint32_t variationIDX);
		void UploadUniformFloat4Array	(const std::string& name, const glm::vec4& values,	uint32_t count, uint32_t variationIDX);
		void UploadUniformMat3Array		(const std::string& name, const glm::mat3& values,	uint32_t count, uint32_t variationIDX);
		void UploadUniformMat4Array		(const std::string& name, const glm::mat4& values,	uint32_t count, uint32_t variationIDX);
		void UploadUniformInt	(uint32_t location, const int& value);
		void UploadUniformFloat	(uint32_t location, const float& value);
		void UploadUniformFloat2(uint32_t location, const glm::vec2& value);
		void UploadUniformFloat3(uint32_t location, const glm::vec3& value);
		void UploadUniformFloat4(uint32_t location, const glm::vec4& value);
		void UploadUniformMat3	(uint32_t location, const glm::mat3& value);
		void UploadUniformMat4	(uint32_t location, const glm::mat4& value);
		void UploadUniformIntArray		(uint32_t location, const int32_t* values,		uint32_t count);
		void UploadUniformFloatArray	(uint32_t location, const float* values,		uint32_t count);
		void UploadUniformFloat2Array	(uint32_t location, const glm::vec2& values,	uint32_t count);
		void UploadUniformFloat3Array	(uint32_t location, const glm::vec3& values,	uint32_t count);
		void UploadUniformFloat4Array	(uint32_t location, const glm::vec4& values,	uint32_t count);
		void UploadUniformMat3Array		(uint32_t location, const glm::mat3& values,	uint32_t count);
		void UploadUniformMat4Array		(uint32_t location, const glm::mat4& values,	uint32_t count);



		virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }
		inline const virtual ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
		inline const ShaderResourceList& GetResources() const override { return m_Resources; }
		const size_t GetVariationIndex(ShaderVariations variation) const;
		
		//std::unordered_map<GLenum, std::string> PreProcess(std::string source, std::unordered_map<std::string, UniformAttributes>& uniformAttributes, bool& createObject2World);
		void PreProcess(std::string fileSource, std::vector<std::unordered_map<GLenum, std::string>>& variant2Source, std::unordered_map<std::string, UniformAttributes>& uniformAttributes);
		void CompileAndUploadShader(std::vector<std::unordered_map<GLenum, std::string>> variant2sourceMap);// bool createObj2World);
		
		void Parse(const std::unordered_map<std::string, UniformAttributes>& uniformAttributes, std::unordered_map<GLenum, std::string>& sourceMap);
		void ParseUniform(const std::string& statement, const std::unordered_map<std::string, UniformAttributes>& uniformAttributes);
		
		void ResolveUniforms(uint32_t variationIDX);
		int32_t GetUniformLocation(const std::string& name, uint32_t variationIDX);
		
		void ResolveAndSetUniforms(Buffer buffer, uint32_t variationIDX);
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer, uint32_t variationIDX);
		void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer, uint32_t variationIDX);
		
		std::vector<uint32_t> m_RendererIDs;
		std::string m_Name, m_AssetPath;
		bool m_IsCompute = false;
		bool m_Loaded = false;
		//std::unordered_map<GLenum, std::string> m_ShaderSource;
		std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;
		Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;
		ShaderResourceArrayList m_ResourceArrays;
		ShaderResourceList m_Resources;
		std::vector<std::unordered_map<std::string, GLint>> m_UniformLocationMaps;

		bool m_IsLit = false;
		uint32_t m_VariationFlags = 0;

		void AddVariation(uint32_t variationIndex)
		{
			m_VariationFlags |= (uint32_t)(BIT(variationIndex + 1));
		}
		void AddVariation(ShaderVariations variation)
		{
			m_VariationFlags |= (uint32_t)variation;
		}
		const bool HasVariation(uint32_t variationIndex) const
		{
			return (uint32_t)(BIT(variationIndex + 1)) & m_VariationFlags;
		}
		/*
		const bool HasVariation(ShaderVariations variation) const
		{
			return (uint32_t)variation & m_VariationFlags;
		}*/
	};
}