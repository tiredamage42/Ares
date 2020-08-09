#pragma once


#include "Ares/Events/MouseEvent.h"
#include <glm/glm.hpp>

namespace Ares {

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);
		
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }
		
		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline const float GetRotation() const { return m_Rotation; }

		inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	private:
		void RecalculateViewMatrix();

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		
		// rotation only around z axis for 2d orthographic cameras
		float m_Rotation = 0.0f; 

	};

	class Camera
	{
	public:

		Camera() = default;
		Camera(const glm::mat4& projectionMatrix);
		virtual ~Camera() = default;


		inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
		
		
		inline void SetViewportSize(uint32_t width, uint32_t height) { m_ViewportWidth = width; m_ViewportHeight = height; }

		void Focus();
		void Update();
		void OnEvent(Event& e);


		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

	private:
		bool OnMouseScroll(MouseScrolledEvent& e);

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition();
	protected:


		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);


	private:
		void UpdateCameraView();

		glm::mat4 m_ViewMatrix;

		glm::vec3 m_Position{ 0 }, m_Rotation{ 0 }, m_FocalPoint;
		bool m_Panning, m_Rotating;
		glm::vec2 m_InitialMousePosition;
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		float m_Distance;
		//float m_PanSpeed, m_RotationSpeed, m_ZoomSpeed;

		float m_Pitch = 0, m_Yaw = 0;
		uint32_t m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}
