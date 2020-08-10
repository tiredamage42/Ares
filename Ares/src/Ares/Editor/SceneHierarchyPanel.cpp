
#include "AresPCH.h"
#include "SceneHierarchyPanel.h"

#include <imgui.h>

#include "Ares/Core/Application.h"
#include "Ares/Core/Entity.h"
#include <assimp/scene.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO:
// - Eventually change imgui node IDs to be entity/asset GUID

namespace Ares {

	glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix);

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
		: m_Context(context)
	{
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};
		//if (m_SelectionContext && false)
		//{
		//	// Try and find same entity in new scene
		//	auto& entityMap = m_Context->GetEntityMap();
		//	UUID selectedEntityID = m_SelectionContext.GetUUID();
		//	if (entityMap.find(selectedEntityID) != entityMap.end())
		//		m_SelectionContext = entityMap.at(selectedEntityID);
		//}
	}
	void SceneHierarchyPanel::SetSelected(Entity entity)
	{
		m_SelectionContext = entity;
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		//uint32_t entityCount = 0, meshCount = 0;

		if (m_Context)
		{

			m_Context->m_Registry.each([&](auto entity)
			{
				//Entity entityS = m_Context->EntityConstructor(entity);
				//DrawEntityNode(entityS, entityCount, meshCount);

				Entity e(entity, m_Context.get());
				if (e.HasComponent<IDComponent>())
					DrawEntityNode(e);
				//DrawEntityNode(Entity(entity, m_Context.get()));
			});

			/*auto& sceneEntities = m_Context->m_Entities;
			for (Entity& entity : sceneEntities)
				DrawEntityNode(entity, entityCount, meshCount);*/

			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					m_Context->CreateEntity("Empty Entity");
				}
				ImGui::EndPopup();
			}

			ImGui::End();

			ImGui::Begin("Properties");

			if (m_SelectionContext)
			{
				DrawComponents(m_SelectionContext);

				/*auto mesh = m_SelectionContext;

				{
					auto [translation, rotation, scale] = GetTransformDecomposition(transform);
					ImGui::Text("World Transform");
					ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
					ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
				}
				{
					auto [translation, rotation, scale] = GetTransformDecomposition(localTransform);
					ImGui::Text("Local Transform");
					ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
					ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
				}*/

				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("AddComponentPanel");

				if (ImGui::BeginPopup("AddComponentPanel"))
				{
					if (!m_SelectionContext.HasComponent<CameraComponent>())
					{
						if (ImGui::Button("Camera"))
						{
							m_SelectionContext.AddComponent<CameraComponent>();
							ImGui::CloseCurrentPopup();
						}
					}
					if (!m_SelectionContext.HasComponent<MeshRendererComponent>())
					{
						if (ImGui::Button("Mesh"))
						{
							m_SelectionContext.AddComponent<MeshRendererComponent>();
							ImGui::CloseCurrentPopup();
						}
					}
					if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
					{
						if (ImGui::Button("Sprite Renderer"))
						{
							m_SelectionContext.AddComponent<SpriteRendererComponent>();
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndPopup();
				}
			}
		}


		ImGui::End();


#if TODO
		ImGui::Begin("Mesh Debug");
		if (ImGui::CollapsingHeader(mesh->m_FilePath.c_str()))
		{
			if (mesh->m_IsAnimated)
			{
				if (ImGui::CollapsingHeader("Animation"))
				{
					if (ImGui::Button(mesh->m_AnimationPlaying ? "Pause" : "Play"))
						mesh->m_AnimationPlaying = !mesh->m_AnimationPlaying;

					ImGui::SliderFloat("##AnimationTime", &mesh->m_AnimationTime, 0.0f, (float)mesh->m_Scene->mAnimations[0]->mDuration);
					ImGui::DragFloat("Time Scale", &mesh->m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
				}
			}
		}
		ImGui::End();
#endif
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)//, uint32_t& imguiEntityID, uint32_t& imguiMeshID)
	{
		//const char* name = entity.GetName().c_str();
		//static char imguiName[128];
		//memset(imguiName, 0, 128);
		//sprintf(imguiName, "%s##%d", name, imguiEntityID++);
		//if (ImGui::TreeNode(imguiName))
		//{
		//	MeshRendererComponent& mrComponent = entity.GetComponent<MeshRendererComponent>();
		//	auto mesh = mrComponent.Mesh;
		//	/*auto material = entity->GetMaterial();
		//	const auto& transform = entity->GetTransform();*/

		//	if (mesh)
		//		DrawMeshNode(mesh, imguiMeshID);

		//	ImGui::TreePop();
		//}


		//
		const char* name = entity.GetComponent<TagComponent>().Tag.c_str();

		ImGuiTreeNodeFlags node_flags = (entity == m_SelectionContext ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, node_flags, name);
		if (ImGui::IsItemClicked())
		{

			m_SelectionContext = entity;
			if (m_SelectionChangedCallback)
				m_SelectionChangedCallback(m_SelectionContext);
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				entityDeleted = true;

			ImGui::EndPopup();
		}
		if (opened)
		{
			if (entity.HasComponent<MeshRendererComponent>())
			{
				auto mesh = entity.GetComponent<MeshRendererComponent>().Mesh;
				// if (mesh)
				// 	DrawMeshNode(mesh);
			}

			ImGui::TreePop();
		}

		// Defer deletion until end of node UI
		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (entity == m_SelectionContext)
				m_SelectionContext = {};

			m_EntityDeletedCallback(entity);
		}
	}

	void SceneHierarchyPanel::DrawMeshNode(const Ref<Mesh>& mesh, uint32_t& imguiMeshID)
	{
		static char imguiName[128];
		memset(imguiName, 0, 128);
		sprintf(imguiName, "Mesh##%d", imguiMeshID++);

		// Mesh Hierarchy
		if (ImGui::TreeNode(imguiName))
		{
			auto rootNode = mesh->m_Scene->mRootNode;
			MeshNodeHierarchy(mesh, rootNode);
			ImGui::TreePop();
		}
	}

	static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
	{
		glm::vec3 scale, translation, skew;
		glm::vec4 perspective;
		glm::quat orientation;
		glm::decompose(transform, scale, orientation, translation, skew, perspective);

		return { translation, orientation, scale };
	}

	void SceneHierarchyPanel::MeshNodeHierarchy(const Ref<Mesh>& mesh, aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 localTransform = Mat4FromAssimpMat4(node->mTransformation);
		glm::mat4 transform = parentTransform * localTransform;
		/*for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			uint32_t meshIndex = node->mMeshes[i];
			mesh->m_Submeshes[meshIndex].Transform = transform;
		}*/

		if (ImGui::TreeNode(node->mName.C_Str()))
		{
			{
				auto [translation, rotation, scale] = GetTransformDecomposition(transform);
				ImGui::Text("World Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}
			{
				auto [translation, rotation, scale] = GetTransformDecomposition(localTransform);
				ImGui::Text("Local Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}

			for (uint32_t i = 0; i < node->mNumChildren; i++)
				MeshNodeHierarchy(mesh, node->mChildren[i], transform, level + 1);

			ImGui::TreePop();
		}
	}


		static int s_UIContextID = 0;
		static uint32_t s_Counter = 0;
		static char s_IDBuffer[16];

		static void PushID()
		{
			ImGui::PushID(s_UIContextID++);
			s_Counter = 0;
		}

		static void PopID()
		{
			ImGui::PopID();
			s_UIContextID--;
		}

		static void BeginPropertyGrid()
		{
			PushID();
			ImGui::Columns(2);
		}


#define START_PROP \
		bool modified = false; \
		ImGui::Text(label); \
		ImGui::NextColumn(); \
		ImGui::PushItemWidth(-1); \
		s_IDBuffer[0] = '#'; \
		s_IDBuffer[1] = '#'; \
		memset(s_IDBuffer + 2, 0, 14); \
		itoa(s_Counter++, s_IDBuffer + 2, 16);


#define END_PROP \
		ImGui::PopItemWidth(); \
		ImGui::NextColumn(); \
		return modified;

		static bool Property(const char* label, std::string & value, bool error = false)
		{
		
			START_PROP

			char buffer[256];
			strcpy(buffer, value.c_str());

			if (error)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

			if (ImGui::InputText(s_IDBuffer, buffer, 256))
			{
				value = buffer;
				modified = true;
			}
			if (error)
				ImGui::PopStyleColor();


			END_PROP
		}

		static bool Property(const char* label, const char* value)
		{
			START_PROP
			modified = ImGui::InputText(s_IDBuffer, (char*)value, 256, ImGuiInputTextFlags_ReadOnly);
			END_PROP
		}

		static bool Property(const char* label, int& value)
		{
			START_PROP
			modified = ImGui::DragInt(s_IDBuffer, &value);
			END_PROP
		}

		static bool Property(const char* label, float& value, float delta = 0.1f)
		{
			START_PROP
			modified = ImGui::DragFloat(s_IDBuffer, &value, delta);
			END_PROP
		}

		static bool Property(const char* label, glm::vec2 & value, float delta = 0.1f)
		{
			START_PROP
			modified = ImGui::DragFloat2(s_IDBuffer, glm::value_ptr(value), delta);
			END_PROP
		}

		static bool Property(const char* label, glm::vec3& value, float delta = 0.1f)
		{
			START_PROP
			modified = ImGui::DragFloat3(s_IDBuffer, glm::value_ptr(value), delta);
			END_PROP
		}

		static bool Property(const char* label, glm::vec4& value, float delta = 0.1f)
		{
			START_PROP
			modified = ImGui::DragFloat4(s_IDBuffer, glm::value_ptr(value), delta);
			END_PROP
		}

		static void EndPropertyGrid()
		{
			ImGui::Columns(1);
			PopID();
		}


		template<typename T, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
		{
			if (entity.HasComponent<T>())
			{
				bool removeComponent = false;

				auto& component = entity.GetComponent<T>();
				bool open = ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(T).hash_code()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap, name.c_str());
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				if (ImGui::Button("+"))
				{
					ImGui::OpenPopup("ComponentSettings");
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove component"))
						removeComponent = true;

					ImGui::EndPopup();
				}

				if (open)
				{
					uiFunction(component);
					ImGui::NextColumn();
					ImGui::Columns(1);
					ImGui::TreePop();
				}
				ImGui::Separator();

				if (removeComponent)
					entity.RemoveComponent<T>();
			}
		}










		void SceneHierarchyPanel::DrawComponents(Entity entity)
		{
			ImGui::AlignTextToFramePadding();

			auto id = entity.GetComponent<IDComponent>().ID;

			if (entity.HasComponent<TagComponent>())
			{
				auto& tag = entity.GetComponent<TagComponent>().Tag;
				char buffer[256];
				memset(buffer, 0, 256);
				memcpy(buffer, tag.c_str(), tag.length());
				if (ImGui::InputText("##Tag", buffer, 256))
				{
					tag = std::string(buffer);
				}

			}
			// ID
			ImGui::SameLine();
			ImGui::TextDisabled("%llx", id);
			ImGui::Separator();

			if (entity.HasComponent<TransformComponent>())
			{
				auto& tc = entity.GetComponent<TransformComponent>();
				if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
				{
					auto [translation, rotationQuat, scale] = GetTransformDecomposition(tc);
					glm::vec3 rotation = glm::degrees(glm::eulerAngles(rotationQuat));

					ImGui::Columns(2);
					ImGui::Text("Translation");
					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);

					bool updateTransform = false;

					if (ImGui::DragFloat3("##translation", glm::value_ptr(translation), 0.25f))
					{
						//tc.Transform[3] = glm::vec4(translation, 1.0f);
						updateTransform = true;
					}

					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Text("Rotation");
					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);

					if (ImGui::DragFloat3("##rotation", glm::value_ptr(rotation), 0.25f))
					{
						updateTransform = true;
						// tc.Transform[3] = glm::vec4(translation, 1.0f);
					}

					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Text("Scale");
					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);

					if (ImGui::DragFloat3("##scale", glm::value_ptr(scale), 0.25f))
					{
						updateTransform = true;
					}

					ImGui::PopItemWidth();
					ImGui::NextColumn();

					ImGui::Columns(1);

					if (updateTransform)
					{
						tc.Transform = glm::translate(glm::mat4(1.0f), translation) *
							glm::toMat4(glm::quat(glm::radians(rotation))) *
							glm::scale(glm::mat4(1.0f), scale);
					}

					// ImGui::Text("Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
					// ImGui::Text("Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
					ImGui::TreePop();
				}
				ImGui::Separator();
			}

			DrawComponent<MeshRendererComponent>("Mesh", entity, [](MeshRendererComponent& mc)

				//if (entity.HasComponent<MeshRendererComponent>())
				//{
					/*auto& mc = entity.GetComponent<MeshRendererComponent>();
					if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(MeshRendererComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))*/
				{
					ImGui::Columns(3);
					ImGui::SetColumnWidth(0, 100);
					ImGui::SetColumnWidth(1, 300);
					ImGui::SetColumnWidth(2, 40);
					ImGui::Text("File Path");
					ImGui::NextColumn();
					ImGui::PushItemWidth(-1);
					if (mc.Mesh)
						ImGui::InputText("##meshfilepath", (char*)mc.Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
					else
						ImGui::InputText("##meshfilepath", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);
					ImGui::PopItemWidth();
					ImGui::NextColumn();
					if (ImGui::Button("...##openmesh"))
					{
						std::string file = Application::Get().OpenFile();
						if (!file.empty())
							mc.Mesh = CreateRef<Mesh>(file);
					}
					/*ImGui::NextColumn();
					ImGui::Columns(1);*/

					/*if (mc.Mesh)
						ImGui::InputText("File Path", (char*)mc.Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
					else
						ImGui::InputText("File Path", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);*/
					//ImGui::TreePop();
				//}
				//ImGui::Separator();
			});


			DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cc)

			/*if (entity.HasComponent<CameraComponent>())
			{
				auto& cc = entity.GetComponent<CameraComponent>();
				if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(CameraComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))*/
				{
					// Projection Type
					const char* projTypeStrings[] = { "Perspective", "Orthographic" };
					const char* currentProj = projTypeStrings[(int)cc.Camera.GetProjectionType()];
					if (ImGui::BeginCombo("Projection", currentProj))
					{
						for (int type = 0; type < 2; type++)
						{
							bool is_selected = (currentProj == projTypeStrings[type]);
							if (ImGui::Selectable(projTypeStrings[type], is_selected))
							{
								currentProj = projTypeStrings[type];
								cc.Camera.SetProjectionType((SceneCamera::ProjectionType)type);
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					BeginPropertyGrid();
					// Perspective parameters
					if (cc.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
					{
						float verticalFOV = cc.Camera.GetPerspectiveVerticalFOV();
						if (Property("Vertical FOV", verticalFOV))
							cc.Camera.SetPerspectiveVerticalFOV(verticalFOV);

						float nearClip = cc.Camera.GetPerspectiveNearClip();
						if (Property("Near Clip", nearClip))
							cc.Camera.SetPerspectiveNearClip(nearClip);
						ImGui::SameLine();
						float farClip = cc.Camera.GetPerspectiveFarClip();
						if (Property("Far Clip", farClip))
							cc.Camera.SetPerspectiveFarClip(farClip);
					}

					// Orthographic parameters
					else if (cc.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						float orthoSize = cc.Camera.GetOrthographicSize();
						if (Property("Size", orthoSize))
							cc.Camera.SetOrthographicSize(orthoSize);

						float nearClip = cc.Camera.GetOrthographicNearClip();
						if (Property("Near Clip", nearClip))
							cc.Camera.SetOrthographicNearClip(nearClip);
						ImGui::SameLine();
						float farClip = cc.Camera.GetOrthographicFarClip();
						if (Property("Far Clip", farClip))
							cc.Camera.SetOrthographicFarClip(farClip);
					}

					EndPropertyGrid();

					/*ImGui::TreePop();
				}
				ImGui::Separator();*/
				});



			DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& sr)
			{
			});

			/*if (entity.HasComponent<SpriteRendererComponent>())
			{
				auto& src = entity.GetComponent<SpriteRendererComponent>();
				if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(SpriteRendererComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
				{
					ImGui::TreePop();
				}
				ImGui::Separator();
			}*/

		

	}

}