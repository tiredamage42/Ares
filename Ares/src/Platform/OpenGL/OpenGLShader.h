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

		virtual void Bind(ShaderVariant variant) override;
		virtual void Unbind() const override;

		//virtual uint32_t GetRendererID(ShaderVariant variant) const override { return m_RendererID; }
		virtual uint32_t GetRendererID(ShaderVariant variant) const override;


		virtual const std::string& GetName() const override { return m_Name; }

		virtual void SetInt(const std::string& name, int value, ShaderVariant variant) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count, ShaderVariant variant, bool deleteFromMem = true) override;
		virtual void SetFloat(const std::string& name, float value, ShaderVariant variant) override;
		virtual void SetFloat2(const std::string& name, glm::vec2 value, ShaderVariant variant) override;
		virtual void SetFloat3(const std::string& name, glm::vec3 value, ShaderVariant variant) override;
		virtual void SetFloat4(const std::string& name, glm::vec4 value, ShaderVariant variant) override;
		virtual void SetMat3(const std::string& name, glm::mat3 value, ShaderVariant variant) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value, ShaderVariant variant) override;

		virtual void SetIntFromRenderThread(const std::string& name, int value, ShaderVariant variant) override;
		virtual void SetIntArrayFromRenderThread(const std::string& name, int* values, uint32_t count, ShaderVariant variant) override;
		virtual void SetFloatFromRenderThread(const std::string& name, float value, ShaderVariant variant) override;
		virtual void SetFloat2FromRenderThread(const std::string& name, glm::vec2 value, ShaderVariant variant) override;
		virtual void SetFloat3FromRenderThread(const std::string& name, glm::vec3 value, ShaderVariant variant) override;
		virtual void SetFloat4FromRenderThread(const std::string& name, glm::vec4 value, ShaderVariant variant) override;
		virtual void SetMat3FromRenderThread(const std::string& name, glm::mat3 value, ShaderVariant variant) override;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, ShaderVariant variant) override;


		/*
		virtual void SetInt(size_t hash, int value, ShaderVariant variant) override;
		virtual void SetIntArray(size_t hash, int* values, uint32_t count, ShaderVariant variant, bool deleteFromMem = true) override;
		virtual void SetFloat(size_t hash, float value, ShaderVariant variant) override;
		virtual void SetFloat2(size_t hash, glm::vec2 value, ShaderVariant variant) override;
		virtual void SetFloat3(size_t hash, glm::vec3 value, ShaderVariant variant) override;
		virtual void SetFloat4(size_t hash, glm::vec4 value, ShaderVariant variant) override;
		virtual void SetMat3(size_t hash, glm::mat3 value, ShaderVariant variant) override;
		virtual void SetMat4(size_t hash, const glm::mat4& value, ShaderVariant variant) override;

		virtual void SetIntFromRenderThread(size_t hash, int value, ShaderVariant variant) override;
		virtual void SetIntArrayFromRenderThread(size_t hash, int* values, uint32_t count, ShaderVariant variant) override;
		virtual void SetFloatFromRenderThread(size_t hash, float value, ShaderVariant variant) override;
		virtual void SetFloat2FromRenderThread(size_t hash, glm::vec2 value, ShaderVariant variant) override;
		virtual void SetFloat3FromRenderThread(size_t hash, glm::vec3 value, ShaderVariant variant) override;
		virtual void SetFloat4FromRenderThread(size_t hash, glm::vec4 value, ShaderVariant variant) override;
		virtual void SetMat3FromRenderThread(size_t hash, glm::mat3 value, ShaderVariant variant) override;
		virtual void SetMat4FromRenderThread(size_t hash, const glm::mat4& value, ShaderVariant variant) override;
		*/












		//virtual void SetVSMaterialUniformBuffer(Buffer buffer, ShaderVariant variant) override;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer, ShaderVariant variant) override;

		//virtual void SetMaterialResources(std::unordered_map<std::string, Ref<Texture>> name2Tex, ShaderVariant variant) override;
		//virtual void SetMaterialResources(std::unordered_map<size_t, Ref<Texture>> name2Tex, ShaderVariant variant) override;


		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;

		//inline virtual const std::unordered_map<std::string, PublicUniformAttributes>& GetPublicUniforms() const override 
		/*
		inline virtual const std::unordered_map<size_t, PublicUniformAttributes>& GetPublicUniforms() const override
		{
			return m_PublicUniforms;
		}
		*/
		
		//virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

	protected:
		/*
		void UploadUniformInt		(size_t hash, int value, ShaderVariant variant);
		void UploadUniformIntArray	(size_t hash, int* values, uint32_t count, ShaderVariant variant);
		void UploadUniformFloat		(size_t hash, float value, ShaderVariant variant);
		void UploadUniformFloat2	(size_t hash, glm::vec2 value, ShaderVariant variant);
		void UploadUniformFloat3	(size_t hash, glm::vec3 value, ShaderVariant variant);
		void UploadUniformFloat4	(size_t hash, glm::vec4 value, ShaderVariant variant);
		void UploadUniformMat3		(size_t hash, glm::mat3 value, ShaderVariant variant);
		void UploadUniformMat4		(size_t hash, const glm::mat4& value, ShaderVariant variant);
		*/
		void UploadUniformInt(const std::string& name, int value, ShaderVariant variant);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count, ShaderVariant variant);
		void UploadUniformFloat(const std::string& name, float value, ShaderVariant variant);
		void UploadUniformFloat2(const std::string& name, glm::vec2 value, ShaderVariant variant);
		void UploadUniformFloat3(const std::string& name, glm::vec3 value, ShaderVariant variant);
		void UploadUniformFloat4(const std::string& name, glm::vec4 value, ShaderVariant variant);
		void UploadUniformMat3(const std::string& name, glm::mat3 value, ShaderVariant variant);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value, ShaderVariant variant);


		void UploadUniformInt(uint32_t location, int value);
		void UploadUniformIntArray(uint32_t location, int* values, uint32_t count);
		void UploadUniformFloat(uint32_t location, float value);
		void UploadUniformFloat2(uint32_t location, glm::vec2 value);
		void UploadUniformFloat3(uint32_t location, glm::vec3 value);
		void UploadUniformFloat4(uint32_t location, glm::vec4 value);
		void UploadUniformMat3(uint32_t location, glm::mat3 value);
		void UploadUniformMat4(uint32_t location, const glm::mat4& value);
		void UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count);

		//void UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset, ShaderVariant variant);

	private:

		/*
		struct ShaderVariantBuffers
		{
			//ShaderUniformBufferList m_VSRendererUniformBuffers;
			//ShaderUniformBufferList m_PSRendererUniformBuffers;

			//Ref<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
			Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;

			ShaderResourceList m_Resources;
			ShaderStructList m_Structs;

			//uint32_t m_RendererID;
		};
		*/


		/*inline const ShaderUniformBufferList& GetVSRendererUniforms() const override { return m_VSRendererUniformBuffers; }
		inline const ShaderUniformBufferList& GetPSRendererUniforms() const override { return m_PSRendererUniformBuffers; }*/

		//virtual bool HasVSMaterialUniformBuffer() const override { return (bool)m_VSMaterialUniformBuffer; }
		//virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }
		
		//virtual bool HasVSMaterialUniformBuffer() const override { return (bool)m_VariantBuffers[0].m_VSMaterialUniformBuffer; }
		//virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_VariantBuffers[0].m_PSMaterialUniformBuffer; }
		virtual bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }

		//inline const virtual ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
		//inline const virtual ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }
		
		//inline const virtual ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer(ShaderVariant variant) const override { return *m_VariantBuffers[GetVariantIndex(variant)].m_VSMaterialUniformBuffer; }
		//inline const virtual ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer(ShaderVariant variant) const override { return *m_VariantBuffers[GetVariantIndex(variant)].m_PSMaterialUniformBuffer; }
		inline const virtual ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }

		//inline const ShaderResourceList& GetResources() const override { return m_Resources; }
		//inline const ShaderResourceList& GetResources(ShaderVariant variant) const override { return m_VariantBuffers[GetVariantIndex(variant)].m_Resources; }
		inline const ShaderResourceList& GetResources() const override { return m_Resources; }




		const size_t GetVariantIndex(ShaderVariant variant) const;
		

		


		//static ShaderType ShaderTypeFromString(const std::string& type);

		//std::string ReadShaderFromFile(const std::string& filePath) const;
		//void ReadShaderFromFile(const std::string& filePath);
		void CompileAndUploadShader();



		std::unordered_map<GLenum, std::string> PreProcess(std::string source, std::unordered_map<std::string, UniformAttributes>& uniformAttributes);


		void Parse(const std::unordered_map<std::string, UniformAttributes>& uniformAttributes);
		//void Parse(ShaderVariantBuffers& buffers, ShaderVariant variant);

		/*void ParseUniform(const std::string& statement, ShaderDomain domain);
		void ParseUniformStruct(const std::string& block, ShaderDomain domain);*/

		//void ParseUniform(const std::string& statement, ShaderDomain domain, ShaderVariantBuffers& variantBuffers, ShaderVariant variant, uint32_t& samplers);
		//void ParseUniformStruct(const std::string& block, ShaderDomain domain, ShaderVariantBuffers& variantBuffers);
		
		//void ParseUniform(const std::string& statement, ShaderVariantBuffers& variantBuffers, ShaderVariant variant);
		//void ParseUniformStruct(const std::string& block, ShaderVariantBuffers& variantBuffers);
		void ParseUniform(const std::string& statement, const std::unordered_map<std::string, UniformAttributes>& uniformAttributes);
		//void ParseUniformStruct(const std::string& block);

		


		//ShaderStruct* FindStruct(const std::string& name);
		//ShaderStruct* FindStruct(const std::string& name, ShaderVariantBuffers& variantBuffers);// , ShaderVariant variant);
		//ShaderStruct* FindStruct(const std::string& name);// , ShaderVariant variant);

		

		//void ResolveUniforms();
		//void ResolveUniforms(const ShaderVariantBuffers& buffers, ShaderVariant variant);
		//void ResolveUniforms2(ShaderVariant variant);
		void ResolveUniforms(ShaderVariant variant);

		
		//void ValidateUniforms();

		//void ResolveAndSetResources(const ShaderResourceList& resources, std::unordered_map<size_t, Ref<Texture>> name2Tex, ShaderVariant variant);

		void ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer, ShaderVariant variant);
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer, ShaderVariant variant);

		void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer, ShaderVariant variant);
		//void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset, const std::string& uniformName, ShaderVariant variant);

		//std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		int32_t GetUniformLocation(const std::string& name, ShaderVariant variant);
		
		//void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

		//mutable std::unordered_map<std::string, GLint> m_UniformLocationMap;

		// unique id in opengl
		//uint32_t m_RendererID = 0;

		std::vector<uint32_t> m_RendererIDs;

		//bool m_HasSkinnedVariant;

		std::string m_Name, m_AssetPath;
		//std::string m_ShaderSource;
		bool m_IsCompute = false;

		bool m_Loaded = false;

		std::unordered_map<GLenum, std::string> m_ShaderSource;

		std::vector<ShaderReloadedCallback> m_ShaderReloadedCallbacks;

		/*ShaderUniformBufferList m_VSRendererUniformBuffers;
		ShaderUniformBufferList m_PSRendererUniformBuffers;*/

		//std::vector<Ref<OpenGLShaderUniformBufferDeclaration>> m_VSMaterialUniformBuffers;
		//std::vector<Ref<OpenGLShaderUniformBufferDeclaration>> m_PSMaterialUniformBuffers;
		
		//Ref<OpenGLShaderUniformBufferDeclaration> m_VSMaterialUniformBuffer;
		Ref<OpenGLShaderUniformBufferDeclaration> m_PSMaterialUniformBuffer;

		ShaderResourceArrayList m_ResourceArrays;
		ShaderResourceList m_Resources;


		//ShaderStructList m_Structs;

		std::unordered_set<std::string> m_ShaderFlags;
		
		//std::vector<ShaderVariantBuffers> m_VariantBuffers;
		

		//std::unordered_map<std::string, PublicUniformAttributes> m_PublicUniforms;
		std::vector<std::unordered_map<std::string, GLint>> m_UniformLocationMaps;

		//std::unordered_map<size_t, PublicUniformAttributes> m_PublicUniforms;

		//std::vector<std::unordered_map<size_t, GLint>> m_UniformLocationMaps;

	};
}