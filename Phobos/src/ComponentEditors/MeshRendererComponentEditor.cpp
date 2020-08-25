#include "PhobosPCH.h"
#include <Ares.h>
#include "../MaterialEditor.h"
#include "MeshRendererComponentEditor.h"

namespace Ares
{

	void MeshRendererComponent::OnDrawImGui()
	{
		ImGui::Columns(3);
		ImGui::SetColumnWidth(0, 100);
		ImGui::SetColumnWidth(1, 300);
		ImGui::SetColumnWidth(2, 40);
		ImGui::Text("File Path");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (this->Mesh)
			ImGui::InputText("##meshfilepath", (char*)this->Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
		else
			ImGui::InputText("##meshfilepath", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		ImGui::NextColumn();
		if (ImGui::Button("...##openmesh"))
		{
			std::string file = Application::Get().OpenFile();
			if (!file.empty())
			{
				std::vector<Ref<Material>> materials;
				std::vector<Ref<Animation>> animations;
				this->Mesh = CreateRef<Ares::Mesh>(file, materials, animations);
				this->Materials = materials;
			}
		}
		ImGui::NextColumn();
		ImGui::Columns(1);


		/*
		if (this->Mesh->m_IsAnimated)
		{
			if (ImGui::CollapsingHeader("Animation"))
			{
				if (ImGui::Button(this->Mesh->m_AnimationPlaying ? "Pause" : "Play"))
					this->Mesh->m_AnimationPlaying = !this->Mesh->m_AnimationPlaying;

				ImGui::SliderFloat("##AnimationTime", &this->Mesh->m_AnimationTime, 0.0f, (float)(this->Mesh->GetAnimationDuration()));
				ImGui::DragFloat("Time Scale", &this->Mesh->m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
			}
		}

		*/

		/*if (mc.Mesh)
			ImGui::InputText("File Path", (char*)mc.Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
		else
			ImGui::InputText("File Path", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);*/
			//ImGui::TreePop();
		//}
		//ImGui::Separator();



		ImGui::Separator();
		ImGui::Text("Materials:");

		uint32_t i = 0;
		for (auto material : this->Materials)
		{

			//ImGuiTreeNodeFlags node_flags = (i == 0 ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

			//if (ImGui::CollapsingHeader("Scene View", nullptr, ImGuiTreeNodeFlags_Selected))

			if (ImGui::TreeNodeEx((void*)i, 0, material->GetName().c_str()))
				//if (ImGui::CollapsingHeader(material->GetName().c_str(), nullptr))//, ImGuiTreeNodeFlags_OpenOnArrow))
			{
				MaterialEditor::DrawMaterial(material);
				ImGui::TreePop();
			}

			i++;
		}
	}

}