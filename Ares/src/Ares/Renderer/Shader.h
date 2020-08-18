#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Buffer.h"

#include "Ares/Renderer/ShaderUniform.h"
#include "Ares/Renderer/Texture.h"
#include <string>
#include <glm/glm.hpp>

namespace Ares {
	//struct ShaderUniform
	//{

	//};

	//struct ShaderUniformCollection
	//{

	//};

	//enum class UniformType
	//{
	//	None = 0,
	//	Float, Float2, Float3, Float4,
	//	Matrix3x3, Matrix4x4,
	//	Int32, Uint32
	//};

	//struct UniformDecl
	//{
	//	UniformType Type;
	//	std::ptrdiff_t Offset;
	//	std::string Name;
	//};

	//struct UniformBuffer
	//{
	//	// TODO: This currently represents a byte buffer that has been
	//	// packed with uniforms. This was primarily created for OpenGL,
	//	// and needs to be revisted for other rendering APIs. Furthermore,
	//	// this currently does not assume any alignment. This also has
	//	// nothing to do with GL uniform buffers, this is simply a CPU-side
	//	// buffer abstraction.
	//	byte* Buffer;
	//	std::vector<UniformDecl> Uniforms;
	//};

	//struct UniformBufferBase
	//{
	//	virtual const byte* GetBuffer() const = 0;
	//	virtual const UniformDecl* GetUniforms() const = 0;
	//	virtual unsigned int GetUniformCount() const = 0;
	//};

	//template<unsigned int N, unsigned int U>
	//struct UniformBufferDeclaration : public UniformBufferBase
	//{
	//	byte Buffer[N];
	//	UniformDecl Uniforms[U];
	//	std::ptrdiff_t Cursor = 0;
	//	int Index = 0;

	//	virtual const byte* GetBuffer() const override { return Buffer; }
	//	virtual const UniformDecl* GetUniforms() const override { return Uniforms; }
	//	virtual unsigned int GetUniformCount() const { return U; }

	//	template<typename T>
	//	void Push(const std::string& name, const T& data) {}

	//	template<>
	//	void Push(const std::string& name, const float& data)
	//	{
	//		Uniforms[Index++] = { UniformType::Float, Cursor, name };
	//		memcpy(Buffer + Cursor, &data, sizeof(float));
	//		Cursor += sizeof(float);
	//	}
	//	template<>
	//	void Push(const std::string& name, const glm::vec3& data)
	//	{
	//		Uniforms[Index++] = { UniformType::Float3, Cursor, name };
	//		memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec3));
	//		Cursor += sizeof(glm::vec3);
	//	}
	//	template<>
	//	void Push(const std::string& name, const glm::vec4& data)
	//	{
	//		Uniforms[Index++] = { UniformType::Float4, Cursor, name };
	//		memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::vec4));
	//		Cursor += sizeof(glm::vec4);
	//	}
	//	template<>
	//	void Push(const std::string& name, const glm::mat4& data)
	//	{
	//		Uniforms[Index++] = { UniformType::Matrix4x4, Cursor, name };
	//		memcpy(Buffer + Cursor, glm::value_ptr(data), sizeof(glm::mat4));
	//		Cursor += sizeof(glm::mat4);
	//	}

	//};



	/*enum class UniformAttribute
	{
		None = BIT(0),
		Public = BIT(1),
		Color = BIT(2),
		Range = BIT(3),
		Toggle = BIT(4),
		BumpMap = BIT(5),
		DefaultValue = BIT(6)
	};

	struct PublicUniformAttributes
	{
		uint32_t Attributes = 0;
		glm::vec2 Range{ 0 };
		byte* DefaultValue;

		bool HasAttribute(UniformAttribute attribute)
		{
			return (uint32_t)attribute & Attributes;
		}
	};*/

	enum class ShaderVariations
	{
		None			= BIT(0),
		Default			= BIT(1),
		DefaultSkinned	= BIT(2),
		Deferred		= BIT(3),
		DeferredSkinned	= BIT(4),
	};

	class Shader
	{
	
	public:
		static uint32_t ShaderVariation2Int(ShaderVariations variation)
		{
			switch (variation)
			{
			case ShaderVariations::None:			return 0;
			case ShaderVariations::Default:			return 0;
			case ShaderVariations::DefaultSkinned:	return 1;
			case ShaderVariations::Deferred:		return 2;
			case ShaderVariations::DeferredSkinned:	return 3;
			default:
				return 0;
			}
		}
		
		static const uint32_t MAX_VARIANTS = 4;

		using ShaderReloadedCallback = std::function<void()>;

		virtual ~Shader() = default;

		virtual void Reload() = 0;

		virtual void Bind(ShaderVariations variation) = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetRendererID(ShaderVariations variant) const = 0;

		//virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) = 0;

		virtual const std::string& GetName() const = 0;

		virtual void SetInt(const std::string& name, int value, ShaderVariations variation) = 0;
		virtual void SetFloat(const std::string& name, float value, ShaderVariations variation) = 0;
		virtual void SetFloat2(const std::string& name, glm::vec2 value, ShaderVariations variation) = 0;
		virtual void SetFloat3(const std::string& name, glm::vec3 value, ShaderVariations variation) = 0;
		virtual void SetFloat4(const std::string& name, glm::vec4 value, ShaderVariations variation) = 0;
		virtual void SetMat3(const std::string& name, glm::mat3 value, ShaderVariations variation) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value, ShaderVariations variation) = 0;

		virtual void SetIntArray(const std::string& name, int32_t* values, uint32_t count, ShaderVariations variation) = 0;
		virtual void SetFloatArray(const std::string& name, float* values, uint32_t count, ShaderVariations variation) = 0;
		virtual void SetFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count, ShaderVariations variation) = 0;
		virtual void SetFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count, ShaderVariations variation) = 0;
		virtual void SetFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count, ShaderVariations variation) = 0;
		virtual void SetMat3Array(const std::string& name, const glm::mat3& values, uint32_t count, ShaderVariations variation) = 0;
		virtual void SetMat4Array(const std::string& name, const glm::mat4& values, uint32_t count, ShaderVariations variation) = 0;


		/*
		virtual void SetIntFromRenderThread(const std::string& name, int value, ShaderVariations variation) = 0;
		virtual void SetFloatFromRenderThread(const std::string& name, float value, ShaderVariations variation) = 0;
		virtual void SetFloat2FromRenderThread(const std::string& name, glm::vec2 value, ShaderVariations variation) = 0;
		virtual void SetFloat3FromRenderThread(const std::string& name, glm::vec3 value, ShaderVariations variation) = 0;
		virtual void SetFloat4FromRenderThread(const std::string& name, glm::vec4 value, ShaderVariations variation) = 0;
		virtual void SetMat3FromRenderThread(const std::string& name, glm::mat3 value, ShaderVariations variation) = 0;
		virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, ShaderVariations variation) = 0;
		virtual void SetIntArrayFromRenderThread(const std::string& name, int* values, uint32_t count, ShaderVariations variation) = 0;
		*/

		/*
		virtual void SetInt		(size_t hash, int value, ShaderVariant variant) = 0;
		virtual void SetIntArray(size_t hash, int* values, uint32_t count, ShaderVariant variant, bool deleteFromMem = true) = 0;
		virtual void SetFloat	(size_t hash, float value, ShaderVariant variant) = 0;
		virtual void SetFloat2	(size_t hash, glm::vec2 value, ShaderVariant variant) = 0;
		virtual void SetFloat3	(size_t hash, glm::vec3 value, ShaderVariant variant) = 0;
		virtual void SetFloat4	(size_t hash, glm::vec4 value, ShaderVariant variant) = 0;
		virtual void SetMat3	(size_t hash, glm::mat3 value, ShaderVariant variant) = 0;
		virtual void SetMat4	(size_t hash, const glm::mat4& value, ShaderVariant variant) = 0;

		virtual void SetIntFromRenderThread		(size_t hash, int value, ShaderVariant variant) = 0;
		virtual void SetIntArrayFromRenderThread(size_t hash, int* values, uint32_t count, ShaderVariant variant) = 0;
		virtual void SetFloatFromRenderThread	(size_t hash, float value, ShaderVariant variant) = 0;
		virtual void SetFloat2FromRenderThread	(size_t hash, glm::vec2 value, ShaderVariant variant) = 0;
		virtual void SetFloat3FromRenderThread	(size_t hash, glm::vec3 value, ShaderVariant variant) = 0;
		virtual void SetFloat4FromRenderThread	(size_t hash, glm::vec4 value, ShaderVariant variant) = 0;
		virtual void SetMat3FromRenderThread	(size_t hash, glm::mat3 value, ShaderVariant variant) = 0;
		virtual void SetMat4FromRenderThread	(size_t hash, const glm::mat4& value, ShaderVariant variant) = 0;
		*/









		//virtual void SetVSMaterialUniformBuffer(Buffer buffer, ShaderVariant variant) = 0;
		virtual void SetPSMaterialUniformBuffer(Buffer buffer, ShaderVariations variation) = 0;
		
		//virtual void SetMaterialResources( std::unordered_map<std::string, Ref<Texture>> name2Tex, ShaderVariant variant) = 0;
		//virtual void SetMaterialResources(std::unordered_map<size_t, Ref<Texture>> name2Tex, ShaderVariant variant) = 0;


		/*virtual const ShaderUniformBufferList& GetVSRendererUniforms() const = 0;
		virtual const ShaderUniformBufferList& GetPSRendererUniforms() const = 0;*/

		//virtual bool HasVSMaterialUniformBuffer() const = 0;
		virtual bool HasPSMaterialUniformBuffer() const = 0;

		//virtual const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer(ShaderVariant variant) const = 0;
		//virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer(ShaderVariant variant) const = 0;
		virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;

		//virtual const ShaderResourceList& GetResources(ShaderVariant variant) const = 0;
		virtual const ShaderResourceList& GetResources() const = 0;

		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

		//virtual const std::unordered_map<std::string, PublicUniformAttributes>& GetPublicUniforms() const = 0;
		//virtual const std::unordered_map<size_t, PublicUniformAttributes>& GetPublicUniforms() const = 0;


		static std::vector<Ref<Shader>> s_AllShaders;
		static std::unordered_map<std::string, Ref<Shader>> s_ShaderMap;
		static Ref<Shader> Find(const std::string& filepath);

	private:
		static Ref<Shader> Create(const std::string& filePath);
		//static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	/*class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);

		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name);
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};*/

}