#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/Texture.h"

namespace Ares {
	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2)
	};

	class Material
	{
		friend class MaterialEditor;
	public:
		Material(Ref<Shader> shader, const std::string& name = "");
		Material(Ref<Material> other, const std::string& name = "");
		virtual ~Material();

		inline const Ref<Shader> GetShader() const { return m_Shader; }

		void Bind();

		uint32_t GetFlags() const { return m_MaterialFlags; }

		bool GetFlag(MaterialFlag flag) const { return (uint32_t)flag & m_MaterialFlags; }
		void SetFlag(MaterialFlag flag, bool value) { 
			if (value)
			{
				m_MaterialFlags |= (uint32_t)flag;
			}
			else
			{
				m_MaterialFlags &= ~(uint32_t)flag;
			}
		}
		
		template <typename T>
		void SetValue(const std::string& name, const T& value)
		{
			auto decl = FindUniformDeclaration(name);
			if (!decl)
			{
				//ARES_CORE_WARN("Could not find uniform with name '{0}'", name);
				return;
			}
			m_PSUniformStorageBuffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

		}

		void SetTexture(const std::string& name, Ref<Texture> texture)
		{
			auto decl = FindResourceDeclaration(name);
			if (!decl)
			{
				//ARES_CORE_WARN("Could not find sampler2D uniform with name '{0}'", name);
				return;
			}
			uint32_t slot = decl->GetRegister();

			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
		}

		const std::string& GetName() const { return m_Name; }

	private:
		void CopyMaterial(Ref<Material> other);
		void AllocateStorage();
		void OnShaderReloaded();
		void BindTextures();

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);
	private:
		std::string m_Name;
		Ref<Shader> m_Shader;
		Buffer m_PSUniformStorageBuffer;
		std::vector<Ref<Texture>> m_Textures;
		uint32_t m_MaterialFlags;
	};
}