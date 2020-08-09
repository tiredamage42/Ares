#pragma once
#include "Ares/Renderer/Camera.h"
#include "Ares/Events/MouseEvent.h"

namespace Ares
{
	class EditorCamera : public Camera
	{
	public:

		EditorCamera() = default;
		EditorCamera(const glm::mat4& projectionMatrix);
		virtual ~EditorCamera() = default;
		
		void Focus();
		void Update();
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(uint32_t width, uint32_t height) { m_ViewportWidth = width; m_ViewportHeight = height; }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const ;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

	private:
		void UpdateCameraView();
		
		bool OnMouseScroll(MouseScrolledEvent& e);
		
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition();
		
		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;


	private:

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position{ 0 }, m_Rotation{ 0 }, m_FocalPoint{ 0 };

		bool m_Panning, m_Rotating;
		glm::vec2 m_InitialMousePosition;
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		float m_Distance;
		float m_Pitch = 0, m_Yaw = 0;
		uint32_t m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}