#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Buffer.h"

#include "Ares/Renderer/ShaderUniform.h"
#include "Ares/Renderer/Texture.h"
#include <string>
//#include <glm/glm.hpp>
#include "Ares/Math/Math.h"
namespace Ares {
	
	enum class ShaderVariations
	{
		None				= BIT(0),
		
		Default				= BIT(1), // or unlit
		DefaultSkinned		= BIT(2),
		
		ForwardBase			= BIT(3),
		ForwardBaseSkinned	= BIT(4),
		
		ForwardAdd			= BIT(5),
		ForwardAddSkinned	= BIT(6),

		Deferred			= BIT(7),
		DeferredSkinned		= BIT(8),
	};

	class Shader
	{
	
	public:
		static uint32_t ShaderVariation2Int(ShaderVariations variation)
		{
			switch (variation)
			{
			case ShaderVariations::Default:				return 0;
			case ShaderVariations::DefaultSkinned:		return 1;
			case ShaderVariations::ForwardBase:			return 2;
			case ShaderVariations::ForwardBaseSkinned:	return 3;
			case ShaderVariations::ForwardAdd:			return 4;
			case ShaderVariations::ForwardAddSkinned:	return 5;
			case ShaderVariations::Deferred:			return 6;
			case ShaderVariations::DeferredSkinned:		return 7;
			default:
				return 0;
			}
		}
		
		static const uint32_t MAX_VARIANTS = 8;

		using ShaderReloadedCallback = std::function<void()>;

		virtual ~Shader() = default;
		virtual void Reload() = 0;
		virtual void Bind(ShaderVariations variation) = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetRendererID(ShaderVariations variant) const = 0;
		virtual const bool IsCurrentlyBound() const = 0;

		virtual const std::string& GetName() const = 0;

		virtual void SetInt(const std::string& name, const int& value) = 0;
		virtual void SetFloat(const std::string& name, const float& value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void SetIntArray(const std::string& name, const int32_t* values, uint32_t count) = 0;
		virtual void SetFloatArray(const std::string& name, const float* values, uint32_t count) = 0;
		virtual void SetFloat2Array(const std::string& name, const glm::vec2& values, uint32_t count) = 0;
		virtual void SetFloat3Array(const std::string& name, const glm::vec3& values, uint32_t count) = 0;
		virtual void SetFloat4Array(const std::string& name, const glm::vec4& values, uint32_t count) = 0;
		virtual void SetMat3Array(const std::string& name, const glm::mat3& values, uint32_t count) = 0;
		virtual void SetMat4Array(const std::string& name, const glm::mat4& values, uint32_t count) = 0;
		virtual const bool IsLit() const = 0;
		virtual const bool HasVariation(ShaderVariations variation) const = 0;

		virtual void SetPSMaterialUniformBuffer(Buffer buffer) = 0;		
		virtual bool HasPSMaterialUniformBuffer() const = 0;
		virtual const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const = 0;
		virtual const ShaderResourceList& GetResources() const = 0;
		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) = 0;

		static std::vector<Ref<Shader>> s_AllShaders;
		static std::unordered_map<std::string, Ref<Shader>> s_ShaderMap;
		static Ref<Shader> Find(const std::string& filepath);
	private:
		static Ref<Shader> Create(const std::string& filePath);
	};
}