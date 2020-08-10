#pragma once


namespace Ares
{

	// From glfw3.h

	#define _ARES_MOUSE_BUTTON_0      0
	#define _ARES_MOUSE_BUTTON_1      1
	#define _ARES_MOUSE_BUTTON_2      2
	#define _ARES_MOUSE_BUTTON_3      3
	#define _ARES_MOUSE_BUTTON_4      4
	#define _ARES_MOUSE_BUTTON_5      5
	#define _ARES_MOUSE_BUTTON_6      6
	#define _ARES_MOUSE_BUTTON_7      7
	#define _ARES_MOUSE_BUTTON_LAST   _ARES_MOUSE_BUTTON_7
	#define _ARES_MOUSE_BUTTON_LEFT   _ARES_MOUSE_BUTTON_0
	#define _ARES_MOUSE_BUTTON_RIGHT  _ARES_MOUSE_BUTTON_1
	#define _ARES_MOUSE_BUTTON_MIDDLE _ARES_MOUSE_BUTTON_2

	typedef enum class MouseButtonCode : uint16_t
	{
		// From glfw3.h
		Button0 = _ARES_MOUSE_BUTTON_0,
		Button1 = _ARES_MOUSE_BUTTON_1,
		Button2 = _ARES_MOUSE_BUTTON_2,
		Button3 = _ARES_MOUSE_BUTTON_3,
		Button4 = _ARES_MOUSE_BUTTON_4,
		Button5 = _ARES_MOUSE_BUTTON_5,
		Button6 = _ARES_MOUSE_BUTTON_6,
		Button7 = _ARES_MOUSE_BUTTON_7,

		ButtonLast = _ARES_MOUSE_BUTTON_LAST,
		ButtonLeft = _ARES_MOUSE_BUTTON_LEFT,
		ButtonRight = _ARES_MOUSE_BUTTON_RIGHT,
		ButtonMiddle = _ARES_MOUSE_BUTTON_MIDDLE
	} MouseButton;

	inline std::ostream& operator<<(std::ostream& os, MouseButtonCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}
// from glfw3.h

//#define ARES_MOUSE_BUTTON_0      ::Ares::MouseButton::Button0
//#define ARES_MOUSE_BUTTON_1      ::Ares::MouseButton::Button1
//#define ARES_MOUSE_BUTTON_2      ::Ares::MouseButton::Button2
//#define ARES_MOUSE_BUTTON_3      ::Ares::MouseButton::Button3
//#define ARES_MOUSE_BUTTON_4      ::Ares::MouseButton::Button4
//#define ARES_MOUSE_BUTTON_5      ::Ares::MouseButton::Button5
//#define ARES_MOUSE_BUTTON_6      ::Ares::MouseButton::Button6
//#define ARES_MOUSE_BUTTON_7      ::Ares::MouseButton::Button7
//#define ARES_MOUSE_BUTTON_LAST   ::Ares::MouseButton::ButtonLast
//#define ARES_MOUSE_BUTTON_LEFT   ::Ares::MouseButton::ButtonLeft
//#define ARES_MOUSE_BUTTON_RIGHT  ::Ares::MouseButton::ButtonRight
//#define ARES_MOUSE_BUTTON_MIDDLE ::Ares::MouseButton::ButtonMiddle
