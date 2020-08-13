#pragma once
#include <Ares.h>
#include "imgui/imgui_internal.h"
#include "Ares/Editor/EditorCamera.h"
#include "Ares/Editor/SceneHierarchyPanel.h"

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

		void ShowBoundingBoxes(bool show, bool onTop = false);
		void SelectEntity(Entity entity);

	private:
		void SetPBRMaterialValues(Ref<Material> material) const;//, const glm::mat4& viewProjection) const;
		std::pair<float, float> GetMouseViewportSpace();
		std::pair<glm::vec3, glm::vec3> CastRay(float mx, float my);
	
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		float m_FrameTimeGraph[100];
		int values_offset = 0;
		glm::vec2 m_ViewportSize = { 0,0 };
		
		// 2d
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_Texture, m_SpriteSheet;
		Entity m_SquareEntity;
		//Entity m_CameraEntity;
		//Ref<Scene> m_ActiveScene;
		//Ref<FrameBuffer> m_FrameBuffer;

		int32_t m_MaxQuadsPerDraw = 10000;
		int32_t m_NumberOfSprites = 10;
		glm::vec4 m_SquareColor = { 1.0f, .5f, 0, 1 };
		glm::ivec2 m_SpriteSheetCoord{ 0 }, m_SpriteSize{ 1 };
		
		// 3d
		//float m_MeshScale = 1.0f;
		//Ref<Scene> m_ActiveScene;
		/*int m_GridScale = 16;
		float m_GridSize = 0.025f;*/

		//Ref<Material> m_GridMaterial;

		/*Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_HDRShader;*/

		
		FileSystemWatcher m_FileSystemWatcher;
		Scope<AssetManagerPanel> m_AssetManagerPanel;
		Scope<SceneHierarchyPanel> m_SceneHierarchyPanel;
		

		//Ref<Scene> m_Scene;
		//Ref<Scene> m_SpheresScene;
		Ref<Scene> m_RuntimeScene, m_EditorScene;

		EditorCamera m_EditorCamera;

		//Entity m_MeshEntity;
		//Entity m_CameraEntity;

		//Ref<Mesh> m_PlaneMesh;

		//Ref<Mesh> m_Mesh;
		//Ref<Mesh> m_CubeMesh, m_PlaneMesh;
		//Ref<Texture2D> m_BRDFLUT;

		//Ref<Material> m_SphereBaseMaterial, m_MeshBaseMaterial;
		Ref<Material> m_MeshBaseMaterial;

		//Ref<Material> m_PBRMaterialStatic, m_PBRMaterialAnim;
		/*std::vector<Ref<MaterialInstance>> m_MetalSphereMaterialInstances;
		std::vector<Ref<MaterialInstance>> m_DielectricSphereMaterialInstances;*/

		struct AlbedoInput
		{
			// Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
			glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; 
			Ref<Texture2D> TextureMap;
			bool SRGB = true;
			bool UseTexture = false;
		};
		AlbedoInput m_AlbedoInput;

		struct NormalInput
		{
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		NormalInput m_NormalInput;

		struct MetalnessInput
		{
			float Value = 1.0f;
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		MetalnessInput m_MetalnessInput;

		struct RoughnessInput
		{
			float Value = 0.2f;
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		RoughnessInput m_RoughnessInput;

		//Ref<RenderPass> m_GeoPass, m_CompositePass;
		/*Ref<FrameBuffer> m_FrameBuffer;
		Ref<FrameBuffer> m_FinalPresentBuffer;*/


		//Ref<VertexArray> m_FullScreenQuadVAO;
		
		/*Ref<TextureCube> m_EnvironmentCubeMap;
		Ref<TextureCube> m_EnvironmentIrradiance;
		Camera m_Camera;*/




		//glm::vec4 testColor;
		/*struct Light
		{
			glm::vec3 Direction;
			glm::vec3 Radiance;
		};
		Light m_Light;
		float m_LightMultiplier = 0.3f;*/

		// PBR params
		//float m_Exposure = 1.0f;

		bool m_RadiancePrefilter = false;
		float m_EnvMapRotation = 0.0f;

		enum class SceneType : uint32_t
		{
			Spheres = 0, Model = 1
		};
		SceneType m_SceneType;

		// Editor resources
		Ref<Texture2D> m_CheckerboardTex;
		Ref<Texture2D> m_PlayButtonTex;

		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1; // -1 = no gizmo
		//glm::mat4 m_Transform;
		float m_SnapValue = 0.5f;

		bool m_AllowViewportCameraEvents = false;
		bool m_DrawOnTopBoundingBoxes = false;

		bool m_UIShowBoundingBoxes = false;
		bool m_UIShowBoundingBoxesOnTop = false;


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
		void OnSelected(const SelectedSubmesh& selectionContext);
		void OnEntityDeleted(Entity e);

		Ray CastMouseRay();

		void OnScenePlay();
		void OnSceneStop();

		void UpdateWindowTitle(const std::string& sceneName);

		enum class SelectionMode
		{
			None = 0, Entity = 1, SubMesh = 2
		};

		SelectionMode m_SelectionMode = SelectionMode::Entity;
		std::vector<SelectedSubmesh> m_SelectionContext;
		glm::mat4* m_RelativeTransform = nullptr;

		//std::vector<SelectedSubmesh> m_SelectedSubmeshes;
		glm::mat4* m_CurrentlySelectedTransform = nullptr;

	
	};
}
