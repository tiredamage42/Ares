#include "AresPCH.h"
#include "CameraController.h"
#include "Ares/Core/Input.h"
#include "Ares/Core/Keycodes.h"

namespace Ares {
	
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), 
			m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
			m_Rotation(rotation)
	{

	}
	void OrthographicCameraController::OnUpdate(float deltaTime)
	{
		if (Input::IsKeyPressed(ARES_KEY_A))
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}
		if (Input::IsKeyPressed(ARES_KEY_D))
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}

		if (Input::IsKeyPressed(ARES_KEY_W))
		{
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}
		if (Input::IsKeyPressed(ARES_KEY_S))
		{
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_PositionSpeed * deltaTime;
		}
		
		if (m_Rotation)
		{
			if (Input::IsKeyPressed(ARES_KEY_Q))
				m_CameraRotation += m_RotationSpeed * deltaTime;
			if (Input::IsKeyPressed(ARES_KEY_E))
				m_CameraRotation -= m_RotationSpeed * deltaTime;
			
			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;
			
			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		m_PositionSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ARES_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(ARES_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}
	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * .25f;
		m_ZoomLevel = std::max(m_ZoomLevel, .25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}