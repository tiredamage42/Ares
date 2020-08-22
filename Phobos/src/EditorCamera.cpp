#include "PhobosPCH.h"
#include "EditorCamera.h"

#define M_PI 3.14159f
namespace Ares
{
	const float RAD2DEG = (180.0f / (float)M_PI);
	const float DEG2RAD = ((float)M_PI / 180.0f);
	
	EditorCamera::EditorCamera(const glm::mat4& projectionMatrix)
		: Camera(projectionMatrix)
	{
		/*m_Position = { 0, 0, -2 };
		m_FocalPoint = glm::vec3(0.0f);
		m_Distance = glm::distance(m_Position, m_FocalPoint);
		UpdateCameraView();*/

		m_FocalPoint = glm::vec3(0.0f);
		m_Distance = 2;
		//UpdateCameraView({ 0, 0, -2 }, 0, 0);


		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), { 0, 0, -2 }) * glm::toMat4(glm::quat(glm::vec3(0, 0, 0.0f))));
	}

	/*
	void EditorCamera::SetViewMatrix(const glm::mat4& viewMatrix)
	{
		m_ViewMatrix = viewMatrix;

		//const glm::mat4 inverted = glm::inverse(viewMatrix);
		//glm::vec3 position = glm::vec3(inverted[3]);
		//const glm::vec3 direction = -glm::vec3(inverted[2]);
		//m_Yaw = glm::atan(direction.z, direction.x);
		//m_Pitch = glm::asin(direction.y);
	}

	*/

	/*
	void EditorCamera::UpdateCameraView(const glm::vec3& pos, const float& pitch, const float& yaw)
	{
		
		//m_Position = CalculatePosition();
		//glm::quat orientation = GetOrientation();
		//m_Rotation = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
	
		
		//m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(GetOrientation()));

		glm::quat rot = glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot));
	}
	*/
	/*
	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}
	*/

	float EditorCamera::ZoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	float GetAxis(KeyCode pos, KeyCode neg) 
	{
		float r = 0;
		if (Input::GetKey(pos)) r += 1;
		if (Input::GetKey(neg)) r -= 1;
		return r;
	}
	float ClampAngle(float angle, float min, float max) 
	{
		
		// remap from [0, 360] to [-180, 180]
		float clamped = DEG2RAD * glm::clamp(fmod((angle * RAD2DEG + 180.0f), 360.0f) - 180.0f, min, max);

		return clamped;// +addBack;
	}

	void EditorCamera::Update()
	{
		//bool updated = false;
		if (m_FreeCamMode)
		//if (m_MoveMode == MoveMode::FreeCamera)
		{

			auto [translation, rotation, scale] = Math::GetTransformDecomposition(glm::inverse(m_ViewMatrix));
			
			bool speedup = Input::GetKey(KeyCode::LeftShift);
			
			float rotMult = (speedup ? m_RotSpeedFast : m_RotSpeed) * Time::GetDeltaTime();
			float speedMult = (speedup ? m_MoveSpeedFast : m_MoveSpeed) * Time::GetDeltaTime();
			
			glm::vec3 angles = glm::eulerAngles(rotation);

			//angles.x = fmod(angles.x, 3.14f);

			//if (angles.x > 1.5708f || angles.x < -1.5708f)
			{
				//ARES_LOG("");
			}
			if (angles.x >= M_PI)
			{
				//angles.x -= M_PI;
			}
			else if (angles.x <= -M_PI)
			{
				//angles.x += M_PI;
			}


			
				/*
			int addBack = 0;
			if (angles.x >= 3.14f)
			{

				angles.x = fmod(angles.x, M_PI);
				//angles.x -= M_PI;
				addBack = (int)(angles.x/M_PI);
			}
			else if (angles.x <= -3.14f)
			{

				angles.x = fmod(angles.x, M_PI);
				//angles.x += M_PI;

				addBack = (int)(angles.x/M_PI);
				//addBack = -M_PI;
			}
				*/

			//bool wasPos = angles.x > 0;
			if (angles.z <= -3.14 || angles.z >= 3.14)
			{
				//ARES_LOG(angles.x);
				//angles.x = -(M_PI - angles.x);

				// if neg
				 angles.x = M_PI + angles.x;
			}

			//angles.x = angles.x - GetAxis(KeyCode::Up, KeyCode::Down) * rotMult;
			angles.x = ClampAngle(angles.x - GetAxis(KeyCode::Up, KeyCode::Down) * rotMult, -89, 89);
			//angles.x += M_PI * addBack;

			if (angles.z <= -3.14 || angles.z >= 3.14)
			{
				angles.x = -(M_PI - angles.x);
				//ARES_LOG(angles.x);
			}


			glm::vec3 up = glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));
			float yawSign = -1;// up.y < 0 ? -1.0f : 1.0f;


			float quaternionFlipMultiplier = angles.z >= 3.14f || angles.z <= -3.14f ? -1 : 1;
			/*
			*/
			angles.y += GetAxis(KeyCode::Right, KeyCode::Left) * rotMult * yawSign * quaternionFlipMultiplier;

			rotation = glm::quat(angles);
			//m_Pitch = ClampAngle(m_Pitch - GetAxis(KeyCode::Up, KeyCode::Down) * rotMult, -89, 89);
			//m_Yaw += GetAxis(KeyCode::Right, KeyCode::Left) * rotMult;


			//glm::quat rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
			//glm::quat rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));


			//Quaternion rot = rotation;// GetOrientation();
			//Vector3 
				up = glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));
			Vector3 right = glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f));
			Vector3 fwd = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));
			
			Vector3 sideMove = right * GetAxis(KeyCode::D, KeyCode::A);
			Vector3 upDownMove = up * GetAxis(KeyCode::E, KeyCode::Q);
			Vector3 fwdMove = fwd * GetAxis(KeyCode::W, KeyCode::S);
			
			
			//glm::vec3 translation = glm::vec3(glm::inverse(m_ViewMatrix)[3]);
			translation += (sideMove + upDownMove + fwdMove) * speedMult;
			m_FocalPoint = translation + fwd;
			//m_Position += (sideMove + upDownMove + fwdMove) * speedMult;
			//m_FocalPoint = m_Position + fwd;
			m_Distance = 1;

			//updated = true;
			//UpdateCameraView(translation, -rotation.x, -rotation.y);

			m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation));
		}
		else
		{
			const glm::vec2& mouse = Input::GetMousePosition();// { Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = mouse - m_LastMousePos;
			m_LastMousePos = mouse;
			
			if (Input::GetKey(KeyCode::LeftAlt))
			{
				//if (Input::GetMouseButton(MouseButtonCode::ButtonLeft) || Input::GetMouseButton(MouseButtonCode::ButtonRight))
				{

					delta *= Time::GetDeltaTime();
					/*
					if (m_MoveMode == MoveMode::Pan)
					{
						if (Input::GetMouseButton(MouseButtonCode::ButtonLeft))
						{
							MousePan(delta);
							//updated = true;
						}
					}
					else if (m_MoveMode == MoveMode::Default)
					{
						if (Input::GetMouseButton(MouseButtonCode::ButtonLeft))
						{

							MouseRotate(delta);
							//updated = true;
						}
					}

					*/
					if (Input::GetMouseButton(MouseButtonCode::ButtonLeft))
					{
						if (Input::GetKey(KeyCode::LeftShift) || Input::GetKey(KeyCode::RightShift))
						{
							MousePan(delta);
						}
						else
						{
							MouseRotate(delta);
						}

						//updated = true;
					}

					else if (
						//!updated && 
						Input::GetMouseButton(MouseButtonCode::ButtonRight))
					{
						MouseZoom(delta.y);
						//updated = true;
					}
				}


			}

			//if (updated)
			{
				//UpdateCameraView();
			}
		}


	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ARES_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::StraightenAngles()
	{
		auto [translation, rotation, scale] = Math::GetTransformDecomposition(glm::inverse(m_ViewMatrix));

		glm::vec3 angles = glm::eulerAngles(rotation);
		angles.y = (glm::round((RAD2DEG * angles.y) / 90.0f) * 90.0f) * DEG2RAD;
		angles.x = (glm::round((RAD2DEG * angles.x) / 90.0f) * 90.0f) * DEG2RAD;

		rotation = glm::quat(angles);

		//m_Yaw = (glm::round((RAD2DEG * m_Yaw) / 90.0f) * 90.0f) * DEG2RAD;
		//m_Pitch = (glm::round((RAD2DEG * m_Pitch) / 90.0f) * 90.0f) * DEG2RAD;

		//glm::quat rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
		//glm::quat rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));



		glm::vec3 fwd = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));

		//glm::vec3 translation = glm::inverse(m_ViewMatrix)[3];
		m_FocalPoint = translation + fwd * m_Distance;

		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation));
		//UpdateCameraView();
	}

	void EditorCamera::Focus(const glm::vec3& focusPos, const float& distance)
	{
		auto [translation, rotation, scale] = Math::GetTransformDecomposition(glm::inverse(m_ViewMatrix));
		m_FocalPoint = focusPos;
		if (distance > 0)
		{
			m_Distance = distance;
		}

		//glm::quat rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
		//glm::quat rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));

		glm::vec3 fwd = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 position = m_FocalPoint - fwd * m_Distance;
		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation));

		//UpdateCameraView();
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		//float delta = e.GetYOffset() * Time::GetDeltaTime();;
		MouseZoom(e.GetYOffset() * Time::GetDeltaTime() * 2);
		//UpdateCameraView();
		return false;
	}
	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		//auto [xSpeed, ySpeed] = m_PanSpeed;// PanSpeed();
		//m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
		//m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
		auto [translation, rotation, scale] = Math::GetTransformDecomposition(glm::inverse(m_ViewMatrix));

		//glm::quat rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
		//glm::quat rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));

		glm::vec3 up = glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 right = glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 fwd = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));

		//glm::vec3 translation = glm::inverse(m_ViewMatrix)[3];
		translation += -right * delta.x * m_PanSpeed.x * m_Distance;
		translation += up * delta.y * m_PanSpeed.y * m_Distance;

		m_FocalPoint = translation + fwd * m_Distance;

		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation));
	}
	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		auto [translation, rotation, scale] = Math::GetTransformDecomposition(glm::inverse(m_ViewMatrix));
		
		glm::vec3 angles = glm::eulerAngles(rotation);
		

		//glm::quat rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
		//glm::quat rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));

		glm::vec3 up = glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		float yawSign = up.y < 0 ? -1.0f : 1.0f;
		//m_Yaw -= yawSign * delta.x * m_OrbitSpeed;// RotationSpeed();
		//m_Pitch -= delta.y * m_OrbitSpeed;


		float quaternionFlipMultiplier = angles.z >= 3.14f || angles.z <= -3.14f ? 1 : -1;

		angles.y += yawSign * quaternionFlipMultiplier * delta.x * m_OrbitSpeed;// RotationSpeed();
		//if (angles.z >= 3.14f || angles.z <= -3.14f)
		//{
		//}
		//else
		//{
		//	angles.y -= yawSign * delta.x * m_OrbitSpeed;// RotationSpeed();
		//}
		/*
		if (angles.y > 1.5708f || angles.y < -1.5708f)
		{
			angles.y += yawSign * delta.x * m_OrbitSpeed;// RotationSpeed();
			//angles.y = (angles.y * -1) + angles.y;

			//angles.z = 6.28319f;
			//angles.y = angles.y - (1.5708f * 2);
		}
		else
		{
			angles.y -= yawSign * delta.x * m_OrbitSpeed;// RotationSpeed();
		}
		*/
		/*
		else if (angles.y < -1.5708f)
		{
			//angles.y = (angles.y * -1) + angles.y;
			//angles.y *= -1;
			//angles.z = 6.28319f;
			//angles.y = angles.y + (1.5708f * 2);
		}
		*/
		/*
		*/


		angles.x -= delta.y * m_OrbitSpeed;


		//glm::radians(45);
		rotation = glm::quat(angles);

		
		//rotation = glm::quat({ angles.x, -angles.y, 0 });
		//rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
		//rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));

		glm::vec3 fwd = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));
		
		//auto 
			translation = m_FocalPoint - fwd * m_Distance;

		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation));
	}
	void EditorCamera::MouseZoom(float delta)
	{

		
		auto [translation, rotation, scale] = Math::GetTransformDecomposition(glm::inverse(m_ViewMatrix));
		//glm::vec3 fwd = GetForwardDirection();
		//glm::quat rotation = glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
		//glm::quat rotation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));

		glm::vec3 fwd = glm::rotate(rotation, glm::vec3(0.0f, 0.0f, -1.0f));

		m_Distance -= delta * ZoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += fwd;
			m_Distance = 1.0f;
		}
		//auto 
			translation = m_FocalPoint - fwd * m_Distance;

		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation));
	}

	/*
	glm::vec3 EditorCamera::GetUpDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	glm::vec3 EditorCamera::GetRightDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}
	glm::vec3 EditorCamera::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition()
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}
	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}
	*/
}