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
		//friend class MaterialInstance;
		friend class MaterialEditor;
	public:
		Material(Ref<Shader> shader, const std::string& name = "");
		Material(Ref<Material> other, const std::string& name = "");
		virtual ~Material();

		inline const Ref<Shader> GetShader() const { return m_Shader; }

		void Bind(ShaderVariant variant);

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

			//m_MaterialFlags |= (uint32_t)flag; 
		}
		

		template <typename T>
		void Set(const std::string& name, const T& value)
		{
			auto decl = FindUniformDeclaration(name);
			if (!decl)
			{
				//ARES_CORE_ERROR("Could not find uniform with name '{0}'", name);
				//ARES_CORE_ASSERT(false, "");
				return;
			}

			auto& buffer = GetUniformBufferTarget(decl);
			buffer.Write((byte*)&value, decl->GetSize(), decl->GetOffset());

			/*for (auto mi : m_MaterialInstances)
				mi->OnMaterialValueUpdated(decl);*/
		}

		std::vector<ShaderResourceDeclaration*> GetResourceDeclarations()
		{
			return m_Shader->GetResources(ShaderVariant::Static);
		}
		std::vector<ShaderUniformDeclaration*> GetUniformDeclarations()
		{
			std::vector<ShaderUniformDeclaration*> result;

			if (m_VSUniformStorageBuffer)
			{
				auto& declarations = m_Shader->GetVSMaterialUniformBuffer(ShaderVariant::Static).GetUniformDeclarations();	
				result.insert(result.end(), declarations.begin(), declarations.end());
			}
			if (m_PSUniformStorageBuffer)
			{
				auto& declarations = m_Shader->GetPSMaterialUniformBuffer(ShaderVariant::Static).GetUniformDeclarations();
				result.insert(result.end(), declarations.begin(), declarations.end());
			}
			return result;
		}

		// TODO: ranges for sliders, and color specification for vec4's, toggle for floats

		//void DrawImGui(Ref<Material> material)
		//{
		//	std::vector<ShaderUniformDeclaration*> uniforms = material->GetUniformDeclarations();

		//	for (auto& uniform : uniforms)
		//	{
		//		auto& buffer = material->GetUniformBufferTarget(uniform);
		//		uint32_t offset = uniform->GetOffset();

		//		byte* finalVal;
		//		bool edited = false;
		//		switch (uniform->GetType())
		//		{
		//		case ShaderUniformDeclaration::Type::FLOAT32:
		//			float value = *(float*)&buffer.Data[offset];

		//			// draw
		//			edited = false;

		//			if (edited) finalVal = (byte*)&value;
		//			break;
		//		case ShaderUniformDeclaration::Type::INT32:
		//			int32_t value = *(int32_t*)&buffer.Data[offset];

		//			// draw
		//			edited = false;

		//			if (edited) finalVal = (byte*)&value;
		//			break;
		//		case ShaderUniformDeclaration::Type::VEC2:
		//			glm::vec2 value = *(glm::vec2*) & buffer.Data[offset];

		//			// draw
		//			edited = false;

		//			if (edited) finalVal = (byte*)&value;
		//			break;
		//		case ShaderUniformDeclaration::Type::VEC3:
		//			glm::vec3 value = *(glm::vec3*) & buffer.Data[offset];

		//			// draw
		//			edited = false;

		//			if (edited) finalVal = (byte*)&value;
		//			break;
		//		case ShaderUniformDeclaration::Type::VEC4:
		//			glm::vec4 value = *(glm::vec4*) & buffer.Data[offset];

		//			// draw
		//			edited = false;

		//			if (edited) finalVal = (byte*)&value;
		//			break;


		//			// no mat3/4 or structs for now....
		//		}
		//		if (edited)
		//		{
		//			buffer.Write(finalVal, uniform->GetSize(), offset);
		//		}
		//	}

		//	std::vector<ShaderResourceDeclaration*> resources = GetResourceDeclarations();

		//	for (auto& decl : resources)
		//	{
		//		std::string name = decl->GetName();
		//		uint32_t slot = decl->GetRegister();

		//		Ref<Texture> tex = nullptr;

		//		if (slot < m_Textures.size())
		//		{
		//			tex = m_Textures[slot];
		//		}


		//		if (ImGui::CollapsingHeader(name, nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		//		{
		//			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
		//			ImGui::Image(tex ? (void*)(intptr_t)tex->GetRendererID() : (void*)(intptr_t)m_CheckerboardTex->GetRendererID(), ImVec2(64, 64));

		//			// Drag 'n' Drop Support
		//			if (ImGui::BeginDragDropTarget())
		//			{
		//				auto data = ImGui::AcceptDragDropPayload("selectable");
		//				if (data)
		//				{
		//					std::string file = (char*)data->Data;
		//					auto extension = AssetManager::ParseFiletype(file);
		//					if (extension == "tga" || extension == "png")
		//					{
		//						tex = Texture2D::Create(file, FilterType::Trilinear, true, false);

		//						if (m_Textures.size() <= slot)
		//							m_Textures.resize((size_t)slot + 1);

		//						m_Textures[slot] = tex;
		//					}
		//				}
		//				ImGui::EndDragDropTarget();
		//			}

		//			ImGui::PopStyleVar();
		//			if (ImGui::IsItemHovered())
		//			{
		//				if (tex)
		//				{
		//					ImGui::BeginTooltip();
		//					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		//					ImGui::TextUnformatted(tex->GetPath().c_str());
		//					ImGui::PopTextWrapPos();
		//					ImGui::Image((void*)(intptr_t)tex->GetRendererID(), ImVec2(384, 384));
		//					ImGui::EndTooltip();
		//				}
		//				if (ImGui::IsItemClicked())
		//				{
		//					std::string filename = Application::Get().OpenFile("");
		//					if (filename != "")
		//					{
		//						tex = Texture2D::Create(filename, FilterType::Trilinear, true, false);


		//						if (m_Textures.size() <= slot)
		//							m_Textures.resize((size_t)slot + 1);

		//						m_Textures[slot] = tex;
		//					}
		//				}
		//			}

		//			if (tex)
		//			{
		//			ImGui::SameLine();
		//			ImGui::BeginGroup();
		//				bool srgb = tex->m_SRGB;
		//				if (ImGui::Checkbox("sRGB##AlbedoMap", &tex))
		//				{
		//					tex = Texture2D::Create(tex->GetPath(), FilterType::Trilinear, true, srgb);
		//					
		//					if (m_Textures.size() <= slot)
		//						m_Textures.resize((size_t)slot + 1);

		//					m_Textures[slot] = tex;
		//				}

		//			ImGui::EndGroup();
		//			}
		//		}
		//	}
		//}



		void Set(const std::string& name, Ref<Texture> texture)
		{
			/*
			//uint8_t slot;
			auto decl = FindResourceDeclaration(name);// , slot);
			uint32_t slot = decl->GetRegister();
			if (!decl)
			{
				//ARES_CORE_ERROR("Could not find sampler2D uniform with name '{0}'", name);
				//ARES_CORE_ASSERT(false, "");
				return;
			}

			if (m_Textures.size() <= slot)
				m_Textures.resize((size_t)slot + 1);
			m_Textures[slot] = texture;
			*/

			if (m_TextureMap.find(name) == m_TextureMap.end())
			{
				ARES_CORE_ERROR("Could not find sampler uniform with name '{0}'", name);
				//ARES_CORE_ASSERT(false, "");
				return;
			}


			m_TextureMap[name] = texture;
		}

		void Set(const std::string& name, Ref<Texture2D> texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}

		void Set(const std::string& name, Ref<TextureCube> texture)
		{
			Set(name, (const Ref<Texture>&)texture);
		}
		const std::string& GetName() const { return m_Name; }

	private:
		void CopyMaterial(Ref<Material> other);
		void AllocateStorage();
		void OnShaderReloaded();
		void BindTextures();

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& name);
		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& name);// , uint8_t& samplerSlot);
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* uniformDeclaration);
	private:
		std::string m_Name;
		Ref<Shader> m_Shader;

		//std::unordered_set<MaterialInstance*> m_MaterialInstances;

		Buffer m_VSUniformStorageBuffer;
		Buffer m_PSUniformStorageBuffer;
		//std::vector<Ref<Texture>> m_Textures;

		std::unordered_map<std::string, Ref<Texture>> m_TextureMap;

		uint32_t m_MaterialFlags;
	};


	/*
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
				//ARES_CORE_ERROR("Could not find uniform with name '{0}'", name);
				//ARES_CORE_ASSERT(false, "");
				return;
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
			if (!decl)
			{
				//ARES_CORE_ERROR("Could not find sampler2D uniform with name '{0}'", name);
				//ARES_CORE_ASSERT(false, "");
				return;
			}

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

		void Bind(ShaderVariant variant);

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
	*/


}