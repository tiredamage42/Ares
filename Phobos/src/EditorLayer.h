#pragma once
#include <Ares.h>
//#include "imgui/imgui_internal.h"
#include "Ares/Editor/EditorCamera.h"
//#include "Ares/Editor/SceneHierarchyPanel.h"
#include "StatsWindow.h"
namespace Ares
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate() override;// float deltaTime) override;
		virtual void OnImGuiDraw() override;
		virtual void OnEvent(Event& e) override;
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		//void ShowBoundingBoxes(bool show, bool onTop = false);
		//void SelectEntity(Entity entity);
		void DrawSceneViewport();
		void DrawMenu();
		void ToolbarUI();
		
	private:

		StatsWindow m_StatsWindow;

		std::pair<float, float> GetMouseViewportSpace();
		std::pair<Vector3, Vector3> CastRay(float mx, float my);
	
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		//float m_FrameTimeGraph[100];
		//int values_offset = 0;
		Vector2 m_ViewportSize = { 0,0 };
		
		// 2d
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_Texture, m_SpriteSheet;
		Entity m_SquareEntity;
		//Entity m_CameraEntity;
		//Ref<Scene> m_ActiveScene;
		//Ref<FrameBuffer> m_FrameBuffer;

		int32_t m_MaxQuadsPerDraw = 10000;
		int32_t m_NumberOfSprites = 10;
		Vector4 m_SquareColor = { 1.0f, .5f, 0, 1 };
		glm::ivec2 m_SpriteSheetCoord{ 0 }, m_SpriteSize{ 1 };
		
		// 3d
		FileSystemWatcher m_FileSystemWatcher;
		Scope<AssetManagerPanel> m_AssetManagerPanel;
		//Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;
		
		Ref<Scene> m_RuntimeScene, m_EditorScene;

		EditorCamera m_EditorCamera;

		std::vector<Ref<Material>> m_MeshMaterials;

		// Editor resources
		Ref<Texture2D> m_PlayButtonTex;

		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1; // -1 = no gizmo
		float m_SnapValue = 0.5f;

		bool m_AllowViewportCameraEvents = false;
		//bool m_DrawOnTopBoundingBoxes = false;

		//bool m_UIShowBoundingBoxes = false;
		//bool m_UIShowBoundingBoxesOnTop = false;

		bool m_ViewportPanelMouseOver = false;
		bool m_ViewportPanelFocused = false;

		enum class SceneState
		{
			Edit = 0, Play = 1, Pause = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		
		struct SelectedSubmesh
		{
			Entity Entity;
			Submesh* Mesh = nullptr;
			float Distance = 0.0f;
		};
		//void OnSelected(const SelectedSubmesh& selectionContext);
		//void OnEntityDeleted(Entity e);

		Ray CastMouseRay();

		void OnScenePlay();
		void OnSceneStop();

		void UpdateWindowTitle(const std::string& sceneName);

		/*
		enum class SelectionMode
		{
			None = 0, Entity = 1, SubMesh = 2
		};
		*/

		//SelectionMode m_SelectionMode = SelectionMode::Entity;
		//std::vector<SelectedSubmesh> m_SelectionContext;
		//Matrix4* m_RelativeTransform = nullptr;
		//Matrix4* m_CurrentlySelectedTransform = nullptr;

		Entity m_SelectedEntity;
	};
}
