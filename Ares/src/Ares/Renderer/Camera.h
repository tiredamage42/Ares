#pragma once


#include "Ares/Events/MouseEvent.h"
//#include <glm/glm.hpp>
#include "Ares/Math/Math.h"
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
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	};
}
