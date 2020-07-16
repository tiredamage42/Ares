#pragma once


namespace Ares
{
	typedef enum class MouseButtonCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} MouseButton;

	inline std::ostream& operator<<(std::ostream& os, MouseButtonCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}
// from glfw3.h

#define ARES_MOUSE_BUTTON_0      ::Ares::MouseButton::Button0
#define ARES_MOUSE_BUTTON_1      ::Ares::MouseButton::Button1
#define ARES_MOUSE_BUTTON_2      ::Ares::MouseButton::Button2
#define ARES_MOUSE_BUTTON_3      ::Ares::MouseButton::Button3
#define ARES_MOUSE_BUTTON_4      ::Ares::MouseButton::Button4
#define ARES_MOUSE_BUTTON_5      ::Ares::MouseButton::Button5
#define ARES_MOUSE_BUTTON_6      ::Ares::MouseButton::Button6
#define ARES_MOUSE_BUTTON_7      ::Ares::MouseButton::Button7
#define ARES_MOUSE_BUTTON_LAST   ::Ares::MouseButton::ButtonLast
#define ARES_MOUSE_BUTTON_LEFT   ::Ares::MouseButton::ButtonLeft
#define ARES_MOUSE_BUTTON_RIGHT  ::Ares::MouseButton::ButtonRight
#define ARES_MOUSE_BUTTON_MIDDLE ::Ares::MouseButton::ButtonMiddle
