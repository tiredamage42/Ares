
#include "AresPCH.h"
#include "Ares/Renderer/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Ares {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0)
	{
		ARES_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		ARES_PROFILE_FUNCTION();

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
	void OrthographicCamera::RecalculateViewMatrix()
	{
		ARES_PROFILE_FUNCTION();

		// the cameras transform matrix (translation, rotation, then scale)
		glm::mat4 transform = glm::translate(glm::mat4(1.0), m_Position) *
			glm::rotate(glm::mat4(1.0), glm::radians(m_Rotation), glm::vec3(0.0, 0.0, 1.0));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}