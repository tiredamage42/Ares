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
		Ref<FrameBuffer> m_FrameBuffer;
		
		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0,0 };

		int32_t m_MaxQuadsPerDraw = 10000;
		int32_t m_NumberOfSprites = 10;

		float m_FrameTimeGraph[100];
		int values_offset = 0;

		glm::vec4 m_SquareColor = { 1.0f, .5f, 0, 1 };
		glm::ivec2 m_SpriteSheetCoord{ 0 }, m_SpriteSize{ 1 };

	};
}
