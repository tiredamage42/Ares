#include "AresPCH.h"
#include "Ares/Renderer/CameraController.h"
#include "Ares/Core/Input.h"
#include "Ares/Core/Keycodes.h"

namespace Ares {
	
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		
		: m_AspectRatio(aspectRatio), 
			m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
			m_Rotation(rotation)
	{

	}
	void OrthographicCameraController::OnUpdate()//float deltaTime)
	{
		float deltaTime = (float)Time::GetDeltaTime();

		if (Input::GetKey(KeyCode::A))
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}
		if (Input::GetKey(KeyCode::D))
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}

		if (Input::GetKey(KeyCode::W))
		{
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}
		if (Input::GetKey(KeyCode::S))
		{
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}
		
		if (m_Rotation)
		{
			if (Input::GetKey(KeyCode::Q))
				m_CameraRotation += m_RotationSpeed * deltaTime;
			if (Input::GetKey(KeyCode::E))
				m_CameraRotation -= m_RotationSpeed * deltaTime;
			
			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;
			
			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		//m_PositionSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::SetZoomLevel(float level)
	{
		ARES_CORE_ASSERT(level > 0.0f, "Zoom Level has to be higher than 0.0f!");

		// translation speed should scale relative to the zoom level changes;
		float diff = level / m_ZoomLevel; // zoom in -> fast translation, zoom out -> slow translation;
		m_PositionSpeed *= diff;
		m_ZoomLevel = level;
		UpdateProjectionMatrix();
	}


	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ARES_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(ARES_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		//CalculateView();
		UpdateProjectionMatrix();
	}
		
	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		float zoomLevel = m_ZoomLevel - e.GetYOffset() * m_ZoomSpeed;
		/* Ares's current "Zoom Level" definition makes std::clamp confuses:
		 *   - 0.25f Zoom Level -> 4x magnification
		 *   - 2.0f Zoom Level -> 0.5x magnification
		 * so the min and max value of "Zoom Level" and "Magnification Level" is the invertion of each other.
		 * If we set MaxZoomLevel to 0.25f (4x) and MinZoomLevel to 2.0f (0.5x) then the user will confuses
		 * "why Max is smaller than Min??".
		 *
		 * Suggestion: Changing the definition to "Magnification Level".
		 */
		zoomLevel = std::clamp(zoomLevel, m_MinZoomLevel, m_MaxZoomLevel);
		SetZoomLevel(zoomLevel);
		UpdateProjectionMatrix();

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
}