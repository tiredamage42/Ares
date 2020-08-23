
#include "PhobosPCH.h"
#include "SceneHierarchyPanel.h"

namespace Ares {

	static void DrawDragDropToNullParentNode()
	{

		ImVec4 backGroundActive = ImGui::GetStyle().Colors[ImGuiCol_TabActive];
		ImGui::PushStyleColor(ImGuiCol_Header, backGroundActive);
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGui::PopStyleColor();

		if (ImGui::TreeNodeEx("0", node_flags, "Null Parent"))
		{

			if (ImGui::BeginDragDropTarget())
			{
				ImGuiDragDropFlags target_flags = 0;
				//target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
				//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHP_Transform", target_flags))
				{
					//auto* transform = (TransformComponent*)payload->Data;
					auto& e = *(Entity*)payload->Data;

					ARES_CORE_WARN("Null Parent {0}", e.GetComponent<EntityComponent>()->Name);
					//m_IsDragging = false;

					e.GetComponent<TransformComponent>()->SetParent(nullptr);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::Draw(Ref<Scene> scene, Entity& selectedEntity, Entity& doubleClickedEntity, bool& windowFocused)//, Entity& deletedEntity)
	{
		doubleClickedEntity = {};
		//deletedEntity = {};
		ImGui::Begin("Scene Hierarchy");
		windowFocused = ImGui::IsWindowFocused();

		DrawDragDropToNullParentNode();
		scene->ForEachEntity([&](const Entity& entity)
			{
				//bool entityClicked;
				//bool entityDeleted;

					// if not parented
				if (entity.GetComponent<TransformComponent>()->Parent == nullptr)
				{

					DrawEntityNode(entity, selectedEntity, doubleClickedEntity, scene);// , entityClicked, entityDeleted);
				}
			/*
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
			*/
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

	static uint64_t s_LastClicked;
	static double s_LastTimeClicked;
	static const double DOUBLE_CLICK_TIME = .5;

	void SceneHierarchyPanel::DrawEntityNode(const Entity& entity, Entity& selectedEntity, Entity& doubleClickedEntity, Ref<Scene> scene)
	{
		bool clicked = false;
		bool deleted = false;
		const char* name = entity.GetComponent<EntityComponent>()->Name.c_str();



		ImGuiTreeNodeFlags node_flags = (entity == selectedEntity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		if (entity.GetComponent<TransformComponent>()->Children.size() == 0)
		{
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		}


		ImVec4 backGroundActive = ImGui::GetStyle().Colors[ImGuiCol_TabActive];
		ImGui::PushStyleColor(ImGuiCol_Header, backGroundActive);

		bool opened = ImGui::TreeNodeEx((void*)(size_t)(uint32_t)entity, node_flags, name);
		ImGui::PopStyleColor();

		if (ImGui::IsItemClicked())
		{
			clicked = true;

			UUID id = entity.GetComponent<EntityComponent>()->ID;
			if (Time::GetTime() - s_LastTimeClicked <= DOUBLE_CLICK_TIME)
			{
				if (s_LastClicked == id)
				{
					doubleClickedEntity = entity;
					s_LastClicked = 0;
				}
				else
				{
					s_LastClicked = id;
				}
			}
			else
			{
				s_LastClicked = id;
			}
			s_LastTimeClicked = Time::GetTime();
		}
		/*
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			doubleClickedEntity = entity;
		}
		*/
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				deleted = true;

			ImGui::EndPopup();
		}

		ImGuiDragDropFlags src_flags = 0;
		src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;     // Keep the source displayed as hovered
		//src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers; // Because our dragging is local, we disable the feature of opening foreign treenodes/tabs while dragging
		//src_flags |= ImGuiDragDropFlags_SourceNoPreviewTooltip; // Hide the tooltip
		//src_flags |= ImGuiDragDropFlags_SourceAllowNullID;

		if (ImGui::BeginDragDropSource(src_flags))
		{

			if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
				ImGui::Text("Moving \"%s\"", name);

			ImGui::SetDragDropPayload("SHP_Transform", &entity, sizeof(TransformComponent));
			
			//m_IsDragging = true;
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			ImGuiDragDropFlags target_flags = 0;
			//target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
			//target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SHP_Transform", target_flags))
			{
				//auto* transform = (TransformComponent*)payload->Data;
				auto& e = *(Entity*)payload->Data;

				ARES_CORE_WARN("DroppedTransform {0}", e.GetComponent<EntityComponent>()->Name);
				//m_IsDragging = false;

				e.GetComponent<TransformComponent>()->SetParent(entity.GetComponent<TransformComponent>());
			}
			ImGui::EndDragDropTarget();
		}

		if (opened)
		{
			// TODO: show children


			TransformComponent* transform = entity.GetComponent<TransformComponent>();

			for (auto* child : transform->Children)
			{

				DrawEntityNode(scene->m_EntityIDMap[child->Entity], selectedEntity, doubleClickedEntity, scene);
			}
			
			ImGui::TreePop();
		}
		

		if (clicked)
		{
			selectedEntity = entity;
			//selectionChanged = true;
		}

		if (deleted)
		{
			scene->DestroyEntity(entity);
			if (entity == selectedEntity)
				selectedEntity = {};

			//deletedEntity = entity;	
		}


	}
}