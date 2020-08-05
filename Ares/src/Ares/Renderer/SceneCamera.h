#pragma once

#include "Ares/Renderer/Camera.h"

namespace Ares
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		const float GetOrthoSize() const { return m_OrthoSize; }
		void SetOrthoSize(float size) { m_OrthoSize = size; RecalculateProjection(); }

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);
	private:

		void RecalculateProjection();

		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f;
		float m_OrthoFar = 1.0f;

		float m_AspectRatio = 0.0f;

	};	
}