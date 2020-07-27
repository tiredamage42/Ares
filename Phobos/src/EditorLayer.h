#pragma once
#include <Ares.h>

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

	private:
		OrthographicCameraController m_CameraController;
		Ref<Texture2D> m_Texture, m_SpriteSheet;
		//Ref<FrameBuffer> m_FrameBuffer;
		
		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		//glm::vec2 m_ViewportSize = { 0,0 };

		int32_t m_MaxQuadsPerDraw = 10000;
		int32_t m_NumberOfSprites = 10;

		float m_FrameTimeGraph[100];
		int values_offset = 0;

		glm::vec4 m_SquareColor = { 1.0f, .5f, 0, 1 };
		glm::ivec2 m_SpriteSheetCoord{ 0 }, m_SpriteSize{ 1 };



		//Ref<Shader> m_PBRShader;
		//Ref<Shader> m_SimplePBRShader;
		Ref<Shader> m_QuadShader;
		//Ref<Shader> m_HDRShader;
		//Ref<Mesh> m_Mesh;
		//Ref<Mesh> m_SphereMesh;
		//Ref<Texture2D> m_BRDFLUT;

		//struct AlbedoInput
		//{
		//	// Silver, from https://docs.unrealengine.com/en-us/Engine/Rendering/Materials/PhysicallyBased
		//	glm::vec3 Color = { 0.972f, 0.96f, 0.915f }; 
		//	Ref<Texture2D> TextureMap;
		//	bool SRGB = true;
		//	bool UseTexture = false;
		//};
		//AlbedoInput m_AlbedoInput;

		//struct NormalInput
		//{
		//	Ref<Texture2D> TextureMap;
		//	bool UseTexture = false;
		//};
		//NormalInput m_NormalInput;

		//struct MetalnessInput
		//{
		//	float Value = 1.0f;
		//	Ref<Texture2D> TextureMap;
		//	bool UseTexture = false;
		//};
		//MetalnessInput m_MetalnessInput;

		//struct RoughnessInput
		//{
		//	float Value = 0.5f;
		//	Ref<Texture2D> TextureMap;
		//	bool UseTexture = false;
		//};
		//RoughnessInput m_RoughnessInput;

		//Ref<FrameBuffer> m_FrameBuffer;
		Ref<FrameBuffer> m_FinalPresentBuffer;

		/*Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;*/

		Ref<VertexArray> m_QuadVertexArray;
		
		//Ref<TextureCube> m_EnvironmentCubeMap;
		Ref<TextureCube> m_EnvironmentIrradiance;

		Camera m_Camera;


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

		enum class Scene : uint32_t
		{
			Spheres = 0, Model = 1
		};
		Scene m_Scene;

		// Editor resources
		Ref<Texture2D> m_CheckerboardTex;

	};
}
