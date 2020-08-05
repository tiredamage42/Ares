
#include "AresPCH.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Ares
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthoSize = size;
		m_OrthoNear = nearClip;
		m_OrthoFar = farClip;
		RecalculateProjection();
	}
	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{

		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}
	void SceneCamera::RecalculateProjection()
	{
		// left, right, bottom, top, near, far
		float l = -m_OrthoSize * m_AspectRatio * .5f;
		float r =  m_OrthoSize * m_AspectRatio * .5f;
		float b = -m_OrthoSize * .5f;
		float t =  m_OrthoSize * .5f;

		m_ProjectionMatrix = glm::ortho(l, r, b, t, m_OrthoNear, m_OrthoFar);
	
	}
}