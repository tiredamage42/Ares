#include "PhobosPCH.h"
#include "InspectorPanel.h"
#include "EditorGUI.h"
#include "MaterialEditor.h"
//#include <assimp/scene.h>
// TODO:
// - Eventually change imgui node IDs to be entity/asset GUID

namespace Ares
{



	static char s_IDBuffer[16];
	/*
	static int s_UIContextID = 0;
	static uint32_t s_Counter = 0;

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
	*/

	/*

#define START_PROP \
		bool modified = false; \
		ImGui::Text(label); \
		ImGui::NextColumn(); \
		ImGui::PushItemWidth(-1); \
		s_IDBuffer[0] = '#'; \
		s_IDBuffer[1] = '#'; \
		memset(s_IDBuffer + 2, 0, 14); \
		_itoa(s_Counter++, s_IDBuffer + 2, 16);


#define END_PROP \
		ImGui::PopItemWidth(); \
		ImGui::NextColumn(); \
		return modified;
	static bool Property(const char* label, std::string& value, bool error = false)
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

	static bool Property(const char* label, glm::vec2& value, float delta = 0.1f)
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
	*/

	/*
	static void EndPropertyGrid()
	{
		ImGui::Columns(1);
		PopID();
	}
	*/
	
	
	/*
	template <typename T>
	class HasRenderMethod
	{
	private:
		typedef char YesType[1];
		typedef char NoType[2];
		template <typename C> static YesType& test(decltype(&C::OnDrawImGui));
		template <typename C> static NoType& test(...);

	public:
		enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
	};

	template<typename T>
	struct HasRenderMethod
	{
	private:
		typedef std::true_type yes;
		typedef std::false_type no;

		template<typename U> static auto test(int) -> decltype(std::declval<U>().size() == 1, yes());

		template<typename> static no test(...);

	public:

		static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
	};


	*/
	template<typename T>//, typename UIFunction>
	static void InspectorPanel::DrawComponent(const std::string& name, Entity entity)//, UIFunction uiFunction)
	{
		if (entity.HasComponent<T>())
		{
			bool removeComponent = false;

			auto* component = entity.GetComponent<T>();
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
				Component* asC = (Component*)component;
				if (asC)
				{
					asC->OnDrawImGui();
				}
				else
				{
					ARES_CORE_WARN("Component '{0}' does not inherit from Ares::Component, can't draw in ImGui", name);
					ImGui::Text("================");
					ImGui::TextWrapped("Component does not inherit from Ares::Component, can't draw in ImGui");
					ImGui::Text("================");
				}
				//typename std::enable_if<HasRenderMethod<T>::value, std::string>::type

				/*
				*/
				//T::OnDrawImGui(component);
				
				/*
				if (HasRenderMethod<T>::value)
				{
				}
				else
				{
					ImGui::Text("================");
					ImGui::TextWrapped("No 'OnDrawImGui' implemented for this component!");
					ImGui::Text("================");
				}
				*/
				//uiFunction(component);
				
				ImGui::NextColumn();
				ImGui::Columns(1);
				ImGui::TreePop();
			}
			ImGui::Separator();

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}










	void InspectorPanel::DrawComponents(const Entity& entity)
	{
		ImGui::AlignTextToFramePadding();

		auto id = entity.GetComponent<IDComponent>()->ID;

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>()->Tag;
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
			auto* tc = entity.GetComponent<TransformComponent>();
			if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto [translation, rotationQuat, tScale] = Math::GetTransformDecomposition(tc->Transform);
				glm::vec3 pos = translation;
				glm::vec3 scale = tScale;
				glm::quat rot = rotationQuat;
				//auto [translation, rotationQuat, scale] = GetTransformDecomposition(tc->Transform);


				bool updateTransform = false;

				ImGui::Columns(2);

				if (EditorGUI::Vec3Field("Position", pos, [tc, rot, scale](Vector3 v) {
					tc->Transform = Math::GetTRSMatrix(v, rot, scale);
					}))
				{
					updateTransform = true;
				}

					glm::vec3 rotation = glm::degrees(glm::eulerAngles(rotationQuat));
					if (EditorGUI::Vec3Field("Rotation", rotation, [tc, pos, scale](Vector3 v) {
						tc->Transform = Math::GetTRSMatrix(pos, glm::quat(glm::radians(v)), scale);
						}))
					{
						updateTransform = true;
					}

						if (EditorGUI::Vec3Field("Scale", scale, [tc, rot, pos](Vector3 v) {
							tc->Transform = Math::GetTRSMatrix(pos, rot, v);
							}))
						{
							updateTransform = true;
						}

							ImGui::Columns(1);

							/*
							ImGui::Columns(2);
							ImGui::Text("Translation");
							ImGui::NextColumn();
							ImGui::PushItemWidth(-1);


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
							*/

							if (updateTransform)
							{
								tc->Transform = glm::translate(glm::mat4(1.0f), pos) *
									glm::toMat4(glm::quat(glm::radians(rotation))) *
									glm::scale(glm::mat4(1.0f), scale);
							}

							// ImGui::Text("Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
							// ImGui::Text("Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
							ImGui::TreePop();
			}
			ImGui::Separator();
		}

		DrawComponent<MeshRendererComponent>("Mesh", entity);/* , [](MeshRendererComponent* mc)

			//if (entity.HasComponent<MeshRendererComponent>())
			//{
				/auto& mc = entity.GetComponent<MeshRendererComponent>();
				if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(MeshRendererComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))/
			{
				ImGui::Columns(3);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, 300);
				ImGui::SetColumnWidth(2, 40);
				ImGui::Text("File Path");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				if (mc->Mesh)
					ImGui::InputText("##meshfilepath", (char*)mc->Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
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
						mc->Mesh = CreateRef<Mesh>(file, materials);
						mc->Materials = materials;
					}
				}
				ImGui::NextColumn();
				ImGui::Columns(1);

				/
				if (mc->Mesh->m_IsAnimated)
				{
					if (ImGui::CollapsingHeader("Animation"))
					{
						if (ImGui::Button(mc->Mesh->m_AnimationPlaying ? "Pause" : "Play"))
							mc->Mesh->m_AnimationPlaying = !mc->Mesh->m_AnimationPlaying;

						ImGui::SliderFloat("##AnimationTime", &mc->Mesh->m_AnimationTime, 0.0f, (float)(mc->Mesh->GetAnimationDuration()));
						ImGui::DragFloat("Time Scale", &mc->Mesh->m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
					}
				}
				/
				
				
				/if (mc.Mesh)
					ImGui::InputText("File Path", (char*)mc.Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
				else
					ImGui::InputText("File Path", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);/
					//ImGui::TreePop();
				//}
				//ImGui::Separator();



				ImGui::Separator();
				ImGui::Text("Materials:");

				uint32_t i = 0;
				for (auto material : mc->Materials)
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
			});
			*/
		
		DrawComponent<CameraComponent>("Camera", entity);/*, [](CameraComponent* cc)

	/if (entity.HasComponent<CameraComponent>())
	{
		auto& cc = entity.GetComponent<CameraComponent>();
		if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(CameraComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))/
	{
		// Projection Type

		SceneCamera::ProjectionType projection = cc->Camera.GetProjectionType();

		std::function<void(SceneCamera::ProjectionType)> undo = [cc](SceneCamera::ProjectionType v) { cc->Camera.SetProjectionType(v); };
		if (EditorGUI::EnumField("Projection", projection, undo, SceneCamera::AllProjectionTypes, SceneCamera::AllProjectionTypeNames))
			cc->Camera.SetProjectionType(projection);





		/
		const char* projTypeStrings[] = { "Perspective", "Orthographic" };
		const char* currentProj = projTypeStrings[(int)cc->Camera.GetProjectionType()];
		if (ImGui::BeginCombo("Projection", currentProj))
		{
			for (int type = 0; type < 2; type++)
			{
				bool is_selected = (currentProj == projTypeStrings[type]);
				if (ImGui::Selectable(projTypeStrings[type], is_selected, ImGuiItemFlags_SelectableDontClosePopup))
				{
					currentProj = projTypeStrings[type];
					cc->Camera.SetProjectionType((SceneCamera::ProjectionType)type);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		/

		BeginPropertyGrid();
		// Perspective parameters
		if (cc->Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
		{
			float verticalFOV = cc->Camera.GetPerspectiveVerticalFOV();
			if (EditorGUI::FloatField("Vertical FOV", verticalFOV, [cc](float v) { cc->Camera.SetPerspectiveVerticalFOV(v); }))
				cc->Camera.SetPerspectiveVerticalFOV(verticalFOV);

			float nearClip = cc->Camera.GetPerspectiveNearClip();
			if (EditorGUI::FloatField("Near Clip", nearClip, [cc](float v) { cc->Camera.SetPerspectiveNearClip(v); }))
				cc->Camera.SetPerspectiveNearClip(nearClip);

			float farClip = cc->Camera.GetPerspectiveFarClip();
			if (EditorGUI::FloatField("Far Clip", farClip, [cc](float v) { cc->Camera.SetPerspectiveFarClip(v); }))
				cc->Camera.SetPerspectiveFarClip(farClip);
		}

		// Orthographic parameters
		else if (cc->Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
		{

			float orthoSize = cc->Camera.GetOrthographicSize();
			if (EditorGUI::FloatField("Size", orthoSize, [cc](float v) { cc->Camera.SetOrthographicSize(v); }))
				cc->Camera.SetOrthographicSize(orthoSize);

			float nearClip = cc->Camera.GetOrthographicNearClip();
			if (EditorGUI::FloatField("Near Clip", nearClip, [cc](float v) { cc->Camera.SetOrthographicNearClip(v); }))
				cc->Camera.SetOrthographicNearClip(nearClip);

			float farClip = cc->Camera.GetOrthographicFarClip();
			if (EditorGUI::FloatField("Far Clip", farClip, [cc](float v) { cc->Camera.SetOrthographicFarClip(v); }))
				cc->Camera.SetOrthographicFarClip(farClip);

		}

		EndPropertyGrid();

		/ImGui::TreePop();
	}
	ImGui::Separator();/
	});
*/


DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity);/*, [](SpriteRendererComponent* sr)
			{
			});*/

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
	void InspectorPanel::DrawInspectorForEntity(const Entity& entity)
	{

		ImGui::Begin("Inspector");




		
		if (entity)
		{
			DrawComponents(entity);
		

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

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponentPanel");

		if (ImGui::BeginPopup("AddComponentPanel"))
		{
			if (!entity.HasComponent<CameraComponent>())
			{
				if (ImGui::Button("Camera"))
				{
					entity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.HasComponent<MeshRendererComponent>())
			{
				if (ImGui::Button("Mesh"))
				{
					entity.AddComponent<MeshRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::Button("Sprite Renderer"))
				{
					entity.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		}
		ImGui::End();

	}

	//glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix);




}
