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

	Vector2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}
}