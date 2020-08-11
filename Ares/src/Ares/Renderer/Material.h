#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/Texture.h"

/*
	TODO: show material props in imgui

*/
//#include <unordered_set>

namespace Ares {
	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2)
	};

	class Material
	{
		friend class MaterialInstance;
	public:
		Material(Ref<Shader> shader);
		virtual ~Material();

		inline const Ref<Shader> GetShader() const { return m_Shader; }

		void Bind();

		uint32_t GetFlags() const { return m_MaterialFlags; }
		void SetFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }


		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindUniformDeclaration(name);
			if (!decl)
			{
				ARES_CORE_ERROR("Could not find uniform with name '{0}'", name);
				ARES_CORE_ASSERT(false, "");
			}

			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			for (auto mi : m_MaterialInstances)
				mi->OnMaterialValueUpdated(decl);
		}

		void Set(const std::string& name, Ref<Texture> texture)
		{

			uint8_t slot;
			auto decl = FindResourceDeclaration(name, slot);
			//uint32_t slot = decl->GetRegister();
			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, Ref<Texture2D> texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}

		void Set(const std::string& name, Ref<TextureCube> texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		void BindTextures();

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name, uint8_t& samplerSlot);
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
	private:
		Ref<Shader> m_Shader;
		std::unordered_set<MaterialInstance*> m_MaterialInstances;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<Ref<Texture>> m_Textures;

		uint32_t m_MaterialFlags;
	};

	class MaterialInstance
	{
		friend class Material;
	public:
		MaterialInstance(Ref<Material> material, const std::string& name = "");
		virtual ~MaterialInstance();

		uint32_t GetFlags() const { return m_Material->m_MaterialFlags; }
		bool GetFlag(MaterialFlag flag) const { return (uint32_t)flag & m_Material->m_MaterialFlags; }
		void SetFlag(MaterialFlag flag, bool value = true);

		inline const Ref<Shader> GetShader() const { return m_Material->GetShader(); }
		inline const Ref<Material> BaseMaterial() const { return m_Material; }

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = m_Material->FindUniformDeclaration(name);
			if (!decl)
			{
				ARES_CORE_ERROR("Could not find uniform with name '{0}'", name);
				ARES_CORE_ASSERT(false, "");
			}
			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			m_OverriddenValues.insert(name);
		}

		void SetTex(const std::string& name, Ref<Texture> texture)
		{

			uint8_t slot = -1;
			auto decl = m_Material->FindResourceDeclaration(name, slot);
			//uint32_t slot = decl->GetRegister();

			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		void Set(const std::string& name, Ref<Texture2D> texture)
		{
			SetTex(name, texture);
			//Set(name, (Ref<Texture>)texture);
		}

		void Set(const std::string& name, Ref<TextureCube> texture)
		{
			SetTex(name, texture);
			//Set(name, (Ref<Texture>)texture);
		}

		void Bind();

		const std::string& GetName() const { return m_Name; }
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
		void OnMaterialValueUpdated(ShaderUniformDeclaration* decl);
	private:
		Ref<Material> m_Material;
		std::string m_Name;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		std::vector<Ref<Texture>> m_Textures;

		// TODO: This is temporary; come up with a proper system to track overrides
		std::unordered_set<std::string> m_OverriddenValues;
	};

}