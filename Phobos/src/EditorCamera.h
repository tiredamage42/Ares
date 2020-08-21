#pragma once
#include <Ares.h>

namespace Ares
{

	class EditorCamera : public Camera
	{
	public:
		/*
		enum class MoveMode
		{
			Default = 0,
			FreeCamera = 1,
			Pan = 2,
		};

		*/
		EditorCamera() = default;
		EditorCamera(const glm::mat4& projectionMatrix);
		virtual ~EditorCamera() = default;
		void Update();
		void OnEvent(Event& e);
		
		void StraightenAngles();
		void Focus(const glm::vec3& focusPos, const float& distance = -1);

		//inline void SetViewportSize(uint32_t width, uint32_t height) { m_ViewportWidth = width; m_ViewportHeight = height; }
		
		//const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		//inline float GetDistance() const { return m_Distance; }
		//inline void SetDistance(float distance) { m_Distance = distance; }
		glm::mat4 GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

		//void SetViewMatrix(const glm::mat4& viewMatrix);
	private:
		//const glm::vec3& GetPosition() const { return m_Position; }
		//glm::vec3 GetUpDirection();
		//glm::vec3 GetRightDirection();
		//glm::vec3 GetForwardDirection();
		//glm::quat GetOrientation() const;
		//float GetPitch() const { return m_Pitch; }
		//float GetYaw() const { return m_Yaw; }
		//void UpdateCameraView(const glm::vec3& pos, const float& pitch, const float& yaw);
		
		bool OnMouseScroll(MouseScrolledEvent& e);
		
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		//glm::vec3 CalculatePosition();
		//std::pair<float, float> PanSpeed() const;
		//float RotationSpeed() const;
		float ZoomSpeed() const;

	private:

		bool m_FreeCamMode;
		glm::vec2 m_PanSpeed{ 1.0f};
		float m_OrbitSpeed = .8f;

		float m_MoveSpeedFast = 50;
		float m_MoveSpeed = 5;
		float m_RotSpeedFast = 4;
		float m_RotSpeed = 2;
		//MoveMode m_MoveMode = MoveMode::Default;

		glm::mat4 m_ViewMatrix;
		//glm::vec3 m_Position{ 0 }, m_Rotation{ 0 }, m_FocalPoint{ 0 };
		//glm::vec3 m_Position{ 0 }, 
		glm::vec3 m_FocalPoint{ 0 };

		//bool m_Panning, m_Rotating;
		glm::vec2 m_LastMousePos{ 0 };
		//glm::vec3 m_InitialFocalPoint, m_InitialRotation;

		float m_Distance;
		//float m_Pitch = 0, m_Yaw = 0;
		//uint32_t m_ViewportWidth = 1280, m_ViewportHeight = 720;

		friend class EditorLayer;
	};

}