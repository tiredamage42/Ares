#pragma once
#include <Ares.h>
#include "imgui/imgui_internal.h"

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

	private:
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_Texture, m_SpriteSheet;
		
		int m_GridScale = 16;
		float m_GridSize = 0.025f;
		float m_MeshScale = 1.0f;
		
		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0,0 };

		int32_t m_MaxQuadsPerDraw = 10000;
		int32_t m_NumberOfSprites = 10;

		float m_FrameTimeGraph[100];
		int values_offset = 0;

		glm::vec4 m_SquareColor = { 1.0f, .5f, 0, 1 };
		glm::ivec2 m_SpriteSheetCoord{ 0 }, m_SpriteSize{ 1 };

		Ref<Material> m_GridMaterial;

		Ref<Shader> m_SkyboxShader;
		Ref<Shader> m_HDRShader;

		Ref<Mesh> m_Mesh;
		Ref<Mesh> m_CubeMesh, m_PlaneMesh;
		Ref<Texture2D> m_BRDFLUT;

		Ref<Material> m_PBRMaterialStatic, m_PBRMaterialAnim;
		std::vector<Ref<MaterialInstance>> m_MetalSphereMaterialInstances;
		std::vector<Ref<MaterialInstance>> m_DielectricSphereMaterialInstances;

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
			float Value = 0.5f;
			Ref<Texture2D> TextureMap;
			bool UseTexture = false;
		};
		RoughnessInput m_RoughnessInput;

		Ref<FrameBuffer> m_FrameBuffer;
		Ref<FrameBuffer> m_FinalPresentBuffer;

		Ref<VertexArray> m_FullScreenQuadVAO;
		
		Ref<TextureCube> m_EnvironmentCubeMap;
		Ref<TextureCube> m_EnvironmentIrradiance;

		Camera m_Camera;



		glm::vec4 testColor;
		struct Light
		{
			glm::vec3 Direction;
			glm::vec3 Radiance;
		};
		Light m_Light;
		float m_LightMultiplier = 0.3f;

		// PBR params
		float m_Exposure = 1.0f;

		bool m_RadiancePrefilter = false;
		float m_EnvMapRotation = 0.0f;

		enum class SceneType : uint32_t
		{
			Spheres = 0, Model = 1
		};
		SceneType m_SceneType;

		// Editor resources
		Ref<Texture2D> m_CheckerboardTex;

		int m_GizmoType = -1; // -1 = no gizmo
		glm::mat4 m_Transform;

	private:
		void SetPBRMaterialValues(Ref<Material> material, const glm::mat4& viewProjection) const;

	};
}
