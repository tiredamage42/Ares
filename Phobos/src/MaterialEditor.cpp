#include "PhobosPCH.h"
#include "MaterialEditor.h"
#include "EditorGUI.h"
//#include <imgui/imgui.h>
namespace Ares
{
	// TODO: custom cubemap support, need soem way to preview cubemaps
	void MaterialEditor::DrawMaterial(Ref<Material> material)
	{
		std::vector<ShaderUniformDeclaration*> uniforms = material->m_Shader->GetPSMaterialUniformBuffer().GetUniformDeclarations();

		for (auto& uniform : uniforms)
		{
			std::string name = uniform->GetName();
			
			if (!uniform->m_Attributes.HasAttribute(UniformAttribute::Public))
				continue;
			
			auto& buffer = material->m_PSUniformStorageBuffer;
			uint32_t offset = uniform->GetOffset();

			byte* finalVal;
			bool edited = false;
			switch (uniform->GetType())
			{
			case ShaderUniformDeclaration::Type::FLOAT32:
			{
				float value = *(float*)&buffer.Data[offset];
				// draw
				if (uniform->m_Attributes.HasAttribute(UniformAttribute::Toggle))
				{
					auto undoFN = [&buffer, uniform, offset](bool v) { 
						float fv = (float)v;
						buffer.Write((byte*)&fv, uniform->GetSize(), offset); 
					};

					bool isTrue = value >= .5f;
					edited = EditorGUI::ToggleField(name, isTrue, undoFN);
					if (edited)
						value = (float)isTrue;
				}
				else if (uniform->m_Attributes.HasAttribute(UniformAttribute::Range))
				{
					auto undoFN = [&buffer, uniform, offset](float v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::FloatSliderField(name, value, uniform->m_Attributes.Range.x, uniform->m_Attributes.Range.y, undoFN);
				}
				else
				{
					auto undoFN = [&buffer, uniform, offset](float v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::FloatField(name, value, undoFN);
				}
				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::INT32:
			{
				int32_t value = *(int32_t*)&buffer.Data[offset];
				if (uniform->m_Attributes.HasAttribute(UniformAttribute::Toggle))
				{
					auto undoFN = [&buffer, uniform, offset](bool v) {
						int32_t fv = (int32_t)v;
						buffer.Write((byte*)&fv, uniform->GetSize(), offset);
					};

					bool isTrue = (bool)value;
					edited = EditorGUI::ToggleField(name, isTrue, undoFN);
					if (edited)
						value = (int32_t)isTrue;
				}
				else if (uniform->m_Attributes.HasAttribute(UniformAttribute::Range))
				{
					auto undoFN = [&buffer, uniform, offset](int v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::IntSliderField(name, value, (int32_t)uniform->m_Attributes.Range.x, (int32_t)uniform->m_Attributes.Range.y, undoFN);
				}
				else
				{
					auto undoFN = [&buffer, uniform, offset](int v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::IntField(name, value, undoFN);
				}
				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC2:
			{
				Vector2 value = *(Vector2*) & buffer.Data[offset];
				// draw
				if (uniform->m_Attributes.HasAttribute(UniformAttribute::Range))
				{
					auto undoFN = [&buffer, uniform, offset](Vector2 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Vec2SliderField(name, value, uniform->m_Attributes.Range.x, uniform->m_Attributes.Range.y, undoFN);
				}
				else
				{
					auto undoFN = [&buffer, uniform, offset](Vector2 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Vec2Field(name, value, undoFN);
				}
				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC3:
			{
				Vector3 value = *(Vector3*) & buffer.Data[offset];
				// draw
				if (uniform->m_Attributes.HasAttribute(UniformAttribute::Color))
				{
					auto undoFN = [&buffer, uniform, offset](Vector3 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Color3Field(name, value, undoFN);
				}
				else if (uniform->m_Attributes.HasAttribute(UniformAttribute::Range))
				{
					auto undoFN = [&buffer, uniform, offset](Vector3 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Vec3SliderField(name, value, uniform->m_Attributes.Range.x, uniform->m_Attributes.Range.y, undoFN);
				}
				else
				{
					auto undoFN = [&buffer, uniform, offset](Vector3 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Vec3Field(name, value, undoFN);
				}
				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC4:
			{
				Vector4 value = *(Vector4*)&buffer.Data[offset];

				// draw
				if (uniform->m_Attributes.HasAttribute(UniformAttribute::Color))
				{
					auto undoFN = [&buffer, uniform, offset](Vector4 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Color4Field(name, value, undoFN);
				}
				else if (uniform->m_Attributes.HasAttribute(UniformAttribute::Range))
				{
					auto undoFN = [&buffer, uniform, offset](Vector4 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Vec4SliderField(name, value, uniform->m_Attributes.Range.x, uniform->m_Attributes.Range.y, undoFN);
				}
				else
				{
					auto undoFN = [&buffer, uniform, offset](Vector4 v) { buffer.Write((byte*)&v, uniform->GetSize(), offset); };
					edited = EditorGUI::Vec4Field(name, value, undoFN);
				}

				if (edited) finalVal = (byte*)&value;
				break;
			}
			default:
			{
				// no mat3/4 or structs for now....
				ARES_CORE_ERROR("Material Editor Cannot Draw Uniform '{0}' of type {1}", name, uniform->GetType());
				ARES_CORE_ASSERT(false, "");
			}

			}
			if (edited)
			{
				buffer.Write(finalVal, uniform->GetSize(), offset);
			}
		}

		std::vector<ShaderResourceDeclaration*> resources = material->m_Shader->GetResources();

		Ref<Texture2D> checkerboardTex = EditorResources::GetTexture("checkerboard.png");

		for (auto& decl : resources)
		{
			if (decl->GetType() != ShaderResourceDeclaration::Type::TEXTURE2D)
				continue;

			if (!decl->m_Attributes.HasAttribute(UniformAttribute::Public))
				continue;

			const std::string& name = decl->GetName();
			
			uint32_t slot = decl->GetRegister();
			Ref<Texture> tex = material->m_Textures[slot];
			
			bool hasTexture = tex != nullptr;

			void* rendererID = (void*)(intptr_t)(hasTexture ? tex->GetRendererID() : checkerboardTex->GetRendererID());
	
			ImGui::CollapsingHeader(name.c_str(), nullptr, ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected);
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
							material->m_Textures[slot] = tex;
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
							material->m_Textures[slot] = tex;
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
						material->m_Textures[slot] = tex;
					}
					if (ImGui::Button(("Remove##" + name).c_str()))
					{
						material->m_Textures[slot] = nullptr;
					}
					ImGui::EndGroup();

				}
			}
		}
	}
}
