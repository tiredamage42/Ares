
#include "AresPCH.h"
#include "Ares/Renderer/Camera.h"

#include "Ares/Core/Time.h"
#include "Ares/Core/Input.h"

//#include <glfw/glfw3.h>
//#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define M_PI 3.14159f

namespace Ares {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::RecalculateViewMatrix()
	{
		// the cameras transform matrix (translation, rotation, then scale)
		glm::mat4 transform = glm::translate(glm::mat4(1.0), m_Position) *
			glm::rotate(glm::mat4(1.0), glm::radians(m_Rotation), glm::vec3(0.0, 0.0, 1.0));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	// NORMAL CAMERA

	Camera::Camera(const glm::mat4& projectionMatrix)
		: m_ProjectionMatrix(projectionMatrix)
	{
		// Sensible defaults
		/*m_PanSpeed = 0.15f;
		m_RotationSpeed = 0.3f;
		m_ZoomSpeed = 10.0f;*/
		m_Position = { 0, 0, -2 };
		//m_Position = { -100, 100, 100 };
		//m_Rotation = glm::vec3(90.0f, 0.0f, 0.0f);

		m_FocalPoint = glm::vec3(0.0f);
		m_Distance = glm::distance(m_Position, m_FocalPoint);


		/*m_Yaw = 3.0f * (float)M_PI / 4.0f;
		m_Pitch = M_PI / 4.0f;*/

		UpdateCameraView();
	}
	void Camera::UpdateCameraView()
	{
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_Rotation = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	std::pair<float, float> Camera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float Camera::RotationSpeed() const
	{
		return 0.8f;
	}

	float Camera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	void Camera::Focus()
	{
	}

	void Camera::Update()
	{
		if (Input::IsKeyPressed(ARES_KEY_LEFT_ALT))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = mouse - m_InitialMousePosition;
			//glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;

			m_InitialMousePosition = mouse;

			delta *= Time::GetDeltaTime();

			if (Input::IsMouseButtonPressed(ARES_MOUSE_BUTTON_MIDDLE))
				MousePan(delta);

			else if (Input::IsMouseButtonPressed(ARES_MOUSE_BUTTON_LEFT))
				MouseRotate(delta);
			
			else if (Input::IsMouseButtonPressed(ARES_MOUSE_BUTTON_RIGHT))
				MouseZoom(delta.y);
		}

		UpdateCameraView();

		//m_Position = CalculatePosition();

		//glm::quat orientation = GetOrientation();
		//m_Rotation = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);

		////m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1)) * glm::toMat4(glm::conjugate(orientation)) * glm::translate(glm::mat4(1.0f), -m_Position);
		//m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		//m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void Camera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ARES_BIND_EVENT_FN(Camera::OnMouseScroll));
	}

	bool Camera::OnMouseScroll(MouseScrolledEvent& e)
	{

		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		return false;
	}


	void Camera::MousePan(const glm::vec2& delta)
	{

		auto [xSpeed, ySpeed] = PanSpeed();
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;


		/*m_FocalPoint += -GetRightDirection() * delta.x * m_PanSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * m_PanSpeed * m_Distance;*/
	}

	void Camera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;


		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();

		/*m_Yaw += yawSign * delta.x * m_RotationSpeed;
		m_Pitch += delta.y * m_RotationSpeed;*/
	}

	void Camera::MouseZoom(float delta)
	{
		//m_Distance -= delta * m_ZoomSpeed;
		m_Distance -= delta * ZoomSpeed();

		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 Camera::GetUpDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 Camera::GetRightDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::vec3 Camera::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
	glm::vec3 Camera::CalculatePosition()
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}
	glm::quat Camera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}
}