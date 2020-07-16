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

		//virtual ~Input() = default;
		//Input(const Input&) = delete;
		//Input& operator=(const Input&) = delete;

		//inline static bool IsKeyPressed(KeyCode keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		//inline static bool IsMouseButtonPressed(MouseButtonCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		/*inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }*/
		//inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }

		//static Scope<Input> Create();

	protected:
		//Input() = default;


		/*virtual bool IsKeyPressedImpl(KeyCode keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseButtonCode button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;*/

		//virtual std::pair<float, float> GetMousePositionImpl() = 0;

	/*private:
		static Scope<Input> s_Instance;*/
	};
}