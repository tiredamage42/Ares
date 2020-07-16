#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Keycodes.h"
#include "Ares/Core/MouseButtonCodes.h"

namespace Ares {
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseButtonCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}