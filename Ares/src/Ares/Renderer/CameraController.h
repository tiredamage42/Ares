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

		inline const float GetZoomLevel(float level) const { return m_ZoomLevel; }
		//void SetZoomLevel(float level) { m_ZoomLevel = level; CalculateView(); }

		/* Setting zoom level using this function will override min/max zoom level. */
		void SetZoomLevel(float level);

		float GetMagnificationZoomLevel() const { return 1.0f / m_ZoomLevel; }
		void SetMagnificationZoomLevel(float magnificationLevel) { if (magnificationLevel != 0.0f) SetZoomLevel(1.0f / magnificationLevel); }

		bool IsRotationEnabled() const { return m_Rotation; }
		void SetEnableRotation(bool enabled) { m_Rotation = enabled; }


		//const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

		void OnResize(float width, float height);
	private:

		void UpdateProjectionMatrix() { m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel); }

		//void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		//OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;



		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f;

		/*float m_PositionSpeed = 5.0f;
		float m_RotationSpeed = 180.0f;*/

		// TODO: add helper function to get/set those values, or public them;
		float m_PositionSpeed = 2.0f;
		float m_RotationSpeed = 180.0f; //Also in degrees, in the anti-clockwise direction
		float m_ZoomSpeed = 0.25f;
		float m_MaxZoomLevel = 2.0f;
		float m_MinZoomLevel = 0.25f;
	};
}