
#include "AresPCH.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Ares
{

	// 2D
	void SceneCamera::RecalculateProjection()
	{
		// left, right, bottom, top, near, far
		/*float l = -m_OrthographicSize * m_AspectRatio * .5f;
		float r = m_OrthographicSize * m_AspectRatio * .5f;
		float b = -m_OrthographicSize * .5f;
		float t = m_OrthographicSize * .5f;

		m_ProjectionMatrix = glm::ortho(l, r, b, t, m_OrthographicNear, m_OrthographicFar);*/
	
	}








	SceneCamera::SceneCamera()
	{
		//2D
		RecalculateProjection();
		//2D
	}

	SceneCamera::~SceneCamera()
	{
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		//2D
		RecalculateProjection();
		//2D
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{

		//2D
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
		//return;
		//2D

		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			m_ProjectionMatrix = glm::perspectiveFov(m_PerspectiveFOV, (float)width, (float)height, m_PerspectiveNear, m_PerspectiveFar);
			break;
		case ProjectionType::Orthographic:
			float aspect = (float)width / (float)height;
			float width = m_OrthographicSize * aspect;
			float height = m_OrthographicSize;
			m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f);
			break;
		}
	}
}