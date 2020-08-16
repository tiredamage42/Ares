#include "MaterialEditor.h"
#include "EditorGUI.h"
#include <imgui/imgui.h>

namespace Ares
{


	void MaterialEditor::AddTextureToMaterial(Ref<Material> material, Ref<Texture> tex, const uint32_t& slot)
	{
		if (material->m_Textures.size() <= slot)
			material->m_Textures.resize((size_t)slot + 1);

		material->m_Textures[slot] = tex;
	}

	// TODO: ranges for sliders, and color specification for vec4's, toggle for floats
	// TODO: custom cubemap support, need soem way to preview cubemaps
	void MaterialEditor::DrawMaterial(Ref<Material> material)
	{

		std::vector<ShaderUniformDeclaration*> uniforms = material->GetUniformDeclarations();

		for (auto& uniform : uniforms)
		{
			auto& buffer = material->GetUniformBufferTarget(uniform);
			uint32_t offset = uniform->GetOffset();

			byte* finalVal;
			bool edited = false;
			switch (uniform->GetType())
			{
			case ShaderUniformDeclaration::Type::FLOAT32:
			{
				float value = *(float*)&buffer.Data[offset];

				// draw
				edited = EditorGUI::Float(uniform->GetName(), value);

				if (edited) finalVal = (byte*)&value;
				break;

			}
			case ShaderUniformDeclaration::Type::INT32:
			{

				int32_t value = *(int32_t*)&buffer.Data[offset];

				// draw
				edited = EditorGUI::Int(uniform->GetName(), value);

				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC2:
			{

				glm::vec2 value = *(glm::vec2*) & buffer.Data[offset];

				// draw
				edited = EditorGUI::Vec2(uniform->GetName(), value);

				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC3:
			{
				glm::vec3 value = *(glm::vec3*) & buffer.Data[offset];

				// draw
				edited = EditorGUI::Vec3(uniform->GetName(), value);

				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC4:
			{
				glm::vec4 value = *(glm::vec4*)&buffer.Data[offset];

				// draw
				edited = EditorGUI::Vec4(uniform->GetName(), value);

				if (edited) finalVal = (byte*)&value;
				break;
			}


				// no mat3/4 or structs for now....
			}
			if (edited)
			{
				buffer.Write(finalVal, uniform->GetSize(), offset);
			}
		}

		std::vector<ShaderResourceDeclaration*> resources = material->GetResourceDeclarations();

		Ref<Texture2D> checkerboardTex = EditorResources::GetTexture("checkerboard.png");

		for (auto& decl : resources)
		{
			bool is2D = decl->GetType() == ShaderResourceDeclaration::Type::TEXTURE2D;
			if (!is2D)
				continue;

			const std::string& name = decl->GetName();
			uint32_t slot = decl->GetRegister();

			Ref<Texture> tex = nullptr;
			if (slot < material->m_Textures.size())
				tex = material->m_Textures[slot];
			
			bool hasTexture = tex != nullptr;

			void* rendererID = (void*)(intptr_t)(hasTexture ? tex->GetRendererID() : checkerboardTex->GetRendererID());

			if (ImGui::CollapsingHeader(name.c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(rendererID, ImVec2(64, 64));
				
				// Drag 'n' Drop Support
				if (ImGui::BeginDragDropTarget())
				{
					auto data = ImGui::AcceptDragDropPayload("selectable");
					if (data)
					{
						std::string file = (char*)data->Data;
						auto extension = AssetManager::ParseFiletype(file);
						if (extension == "tga" || extension == "png")
						{
							tex = Texture2D::Create(file, FilterType::Trilinear, true, false);
							AddTextureToMaterial(material, tex, slot);
						}
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopStyleVar();

				if (ImGui::IsItemHovered())
				{
					if (hasTexture)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(tex->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image(rendererID, ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Application::Get().OpenFile("");
						if (filename != "")
						{
							tex = Texture2D::Create(filename, FilterType::Trilinear, true, false);
							AddTextureToMaterial(material, tex, slot);
						}
					}
				}

				if (hasTexture)
				{
					ImGui::SameLine();
					ImGui::BeginGroup();
					bool srgb = tex->GetSRGB();
					if (ImGui::Checkbox(("sRGB##" + name).c_str(), &srgb))
					{
						tex = Texture2D::Create(tex->GetPath(), FilterType::Trilinear, true, srgb);
						AddTextureToMaterial(material, tex, slot);
					}

					ImGui::EndGroup();
				}
			}
		}
	}
}
