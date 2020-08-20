#include "AresPCH.h"
#include "Ares/Core/Input.h"
#include "Ares/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Ares {

	void Input::UpdateInputPolls()
	{
		glfwPollEvents();
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		for (uint32_t i = 0; i < KEYCODES_COUNT; i++)
		{
			auto state = glfwGetKey(window, static_cast<int32_t>(ALL_KEYCODES[i]));
			s_KeyPressed[curFrame][i] = state == GLFW_PRESS || state == GLFW_REPEAT;
		}

		for (uint32_t i = 0; i < MOUSECODES_COUNT; i++)
		{
			auto state = glfwGetMouseButton(window, static_cast<int32_t>(ALL_MOUSECODES[i]));
			s_MousePressed[curFrame][i] = state == GLFW_PRESS;
		}

		curFrame = (curFrame + 1) % 2;
	}

	/*bool Input::IsKeyPressed(const KeyCode keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS;
	}
	bool Input::IsKeyHeld(const KeyCode keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool Input::IsMouseButtonPressed(const MouseButtonCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}
	*/
	std::pair<float, float> Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}
}