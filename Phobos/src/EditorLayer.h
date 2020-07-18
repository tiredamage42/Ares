#pragma once
#include <Ares.h>

//#include "imgui/imgui.h"
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

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
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_FrameBuffer;
		
		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0,0 };


		int32_t m_MaxQuadsPerDraw = 10000;
		int32_t m_NumberOfSprites = 10;

		float m_FrameTimeGraph[100];
		int values_offset = 0;









		// NWE =========================
		struct NormalInput
		{
			float Value = 1.0f;
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


		struct Light
		{
			glm::vec3 Direction;
			glm::vec3 Radiance;
		};
		Light m_Light;
		float m_LightMultiplier = 1.0f;


		// pbr params
		//float m_Exposure = 1.0f;
		bool m_RadiancePrefilter = false;
		float m_EnvMapRotation = 0.0f;

		enum class SceneType : uint32_t
		{
			Spheres = 0, Model = 1
		};
		SceneType m_SceneType;


		//editor resources
		Ref<Texture2D> m_CheckerboardTex;

		int m_GizmoTyoe = -1; // -1 = no gizmo
		glm::mat4 m_Transform;
	};
}
