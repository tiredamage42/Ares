
#include "PhobosPCH.h"
#include "SceneHierarchyPanel.h"

namespace Ares {

	void SceneHierarchyPanel::Draw(Ref<Scene> scene, Entity& selectedEntity, bool& windowFocused)//, Entity& deletedEntity)
	{
		//deletedEntity = {};
		ImGui::Begin("Scene Hierarchy");
		windowFocused = ImGui::IsWindowFocused();

		scene->ForEachEntity([&](const Entity& entity)
		{
			bool entityClicked;
			bool entityDeleted;
			DrawEntityNode(entity, selectedEntity, entityClicked, entityDeleted);
			
			if (entityClicked)
			{
				selectedEntity = entity;
				//selectionChanged = true;
			}

			if (entityDeleted)
			{
				scene->DestroyEntity(entity);
				if (entity == selectedEntity)
					selectedEntity = {};

				//deletedEntity = entity;	
			}
		});
	
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				scene->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}
		ImGui::End();

	}

	void SceneHierarchyPanel::DrawEntityNode(const Entity& entity, const Entity& selectedEntity, bool& clicked, bool& deleted)
	{
		clicked = false;
		deleted = false;
		const char* name = entity.GetComponent<TagComponent>()->Tag.c_str();

		ImGuiTreeNodeFlags node_flags = (entity == selectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(size_t)(uint32_t)entity, node_flags, name);
		

		if (ImGui::IsItemClicked())
		{
			clicked = true;
		}
		
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				deleted = true;

			ImGui::EndPopup();
		}
		if (opened)
		{
			// TODO: show children
			
			ImGui::TreePop();
		}		
	}
}