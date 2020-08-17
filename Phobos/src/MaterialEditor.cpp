#include "MaterialEditor.h"
#include "EditorGUI.h"
#include <imgui/imgui.h>

namespace Ares
{


	/*void MaterialEditor::AddTextureToMaterial(Ref<Material> material, Ref<Texture> tex, const uint32_t& slot)
	{
		if (material->m_Textures.size() <= slot)
			material->m_Textures.resize((size_t)slot + 1);

		material->m_Textures[slot] = tex;
	}*/

	// TODO: ranges for sliders, and color specification for vec4's, toggle for floats
	// TODO: custom cubemap support, need soem way to preview cubemaps
	void MaterialEditor::DrawMaterial(Ref<Material> material)
	{

		std::vector<ShaderUniformDeclaration*> uniforms = material->GetUniformDeclarations();
		const std::unordered_map<std::string, PublicUniformAttributes>& publicUniforms = material->GetShader()->GetPublicUniforms();

		
		for (auto& uniform : uniforms)
		{
			std::string name = uniform->GetName();

			if (publicUniforms.find(name) == publicUniforms.end())
				continue;

			PublicUniformAttributes attributes = publicUniforms.at(name);
				


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
				if (attributes.HasAttribute(UniformAttribute::Toggle))
				{
					bool isTrue = value >= .5f;
					edited = EditorGUI::Toggle(name, isTrue);
					if (edited)
						value = (float)isTrue;
				}
				else if (attributes.HasAttribute(UniformAttribute::Range))
				{
					edited = EditorGUI::FloatSlider(name, value, attributes.Range.x, attributes.Range.y);
				}
				else
				{
					edited = EditorGUI::Float(name, value);
				}

				if (edited) finalVal = (byte*)&value;
				break;

			}
			case ShaderUniformDeclaration::Type::INT32:
			{

				int32_t value = *(int32_t*)&buffer.Data[offset];

				// draw
				if (attributes.HasAttribute(UniformAttribute::Toggle))
				{
					bool isTrue = (bool)value;
					edited = EditorGUI::Toggle(name, isTrue);
					if (edited)
						value = (int32_t)isTrue;
				}
				else if (attributes.HasAttribute(UniformAttribute::Range))
				{
					edited = EditorGUI::IntSlider(name, value, (int32_t)attributes.Range.x, (int32_t)attributes.Range.y);
				}
				else
				{
					edited = EditorGUI::Int(name, value);
				}


				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC2:
			{

				glm::vec2 value = *(glm::vec2*) & buffer.Data[offset];

				// draw
				if (attributes.HasAttribute(UniformAttribute::Range))
				{
					edited = EditorGUI::Vec2Slider(name, value, attributes.Range.x, attributes.Range.y);
				}
				else
				{
					edited = EditorGUI::Vec2(name, value);
				}

				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC3:
			{
				glm::vec3 value = *(glm::vec3*) & buffer.Data[offset];

				// draw
				if (attributes.HasAttribute(UniformAttribute::Color))
				{
					edited = EditorGUI::Color3(name, value);
				}
				else if (attributes.HasAttribute(UniformAttribute::Range))
				{
					edited = EditorGUI::Vec3Slider(name, value, attributes.Range.x, attributes.Range.y);
				}
				else
				{
					edited = EditorGUI::Vec3(name, value);
				}


				if (edited) finalVal = (byte*)&value;
				break;
			}
			case ShaderUniformDeclaration::Type::VEC4:
			{
				glm::vec4 value = *(glm::vec4*)&buffer.Data[offset];

				// draw
				if (attributes.HasAttribute(UniformAttribute::Color))
				{
					edited = EditorGUI::Color4(name, value);
				}
				else if (attributes.HasAttribute(UniformAttribute::Range))
				{
					edited = EditorGUI::Vec4Slider(name, value, attributes.Range.x, attributes.Range.y);
				}
				else
				{
					edited = EditorGUI::Vec4(name, value);
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

		std::vector<ShaderResourceDeclaration*> resources = material->GetResourceDeclarations();

		Ref<Texture2D> checkerboardTex = EditorResources::GetTexture("checkerboard.png");

		for (auto& decl : resources)
		{
			bool is2D = decl->GetType() == ShaderResourceDeclaration::Type::TEXTURE2D;
			if (!is2D)
				continue;
			
			const std::string& name = decl->GetName();
			
			if (publicUniforms.find(name) == publicUniforms.end())
				continue;

			PublicUniformAttributes attributes = publicUniforms.at(name);

			Ref<Texture> tex = material->m_TextureMap.at(name);

			if (tex)
			{
				
			}
			else
			{
				// bind white texture (blue texture if normal map)
				//tex = Renderer::GetWhiteTexture();

				/*if (attributes.HasAttribute(UniformAttribute::BumpMap))
				{
					tex = Renderer::GetBlueTexture();
				}*/
			}
				


			//uint32_t slot = decl->GetRegister();
			/*Ref<Texture> tex = nullptr;
			if (slot < material->m_Textures.size())
				tex = material->m_Textures[slot];*/
			
			bool hasTexture = tex != nullptr;

			void* rendererID = (void*)(intptr_t)(hasTexture ? tex->GetRendererID() : checkerboardTex->GetRendererID());

			//if (
				//ImGui::CollapsingHeader(name.c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_CollapsingHeader);
				ImGui::CollapsingHeader(name.c_str(), nullptr, ImGuiTreeNodeFlags_Bullet);
				//)

				
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
							//AddTextureToMaterial(material, tex, slot);
							material->m_TextureMap[name] = tex;
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
							//AddTextureToMaterial(material, tex, slot);
							material->m_TextureMap[name] = tex;
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
						//AddTextureToMaterial(material, tex, slot);
						material->m_TextureMap[name] = tex;
					}

					//ImGui::EndGroup();

					//ImGui::BeginGroup();
					if (ImGui::Button(("Remove##" + name).c_str()))
					{
						material->m_TextureMap[name] = nullptr;
					}
					ImGui::EndGroup();

				}
			}
		}
	}
}
