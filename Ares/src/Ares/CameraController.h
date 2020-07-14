#pragma once

#include "Ares/Renderer/Camera.h"

#include "Ares/Events/ApplicationEvent.h"
#include "Ares/Events/MouseEvent.h"

namespace Ares 
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation=false);
		void OnUpdate(float deltaTime);
		void OnEvent(Event& e);

		inline OrthographicCamera& GetCamera() { return m_Camera; }
		inline const OrthographicCamera& GetCamera() const { return m_Camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;

		float m_PositionSpeed = 5.0f;
		float m_RotationSpeed = 180.0f;
	};
}