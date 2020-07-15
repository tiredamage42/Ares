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

#define ARES_MOUSE_BUTTON_0      ::Ares::Mouse::Button0
#define ARES_MOUSE_BUTTON_1      ::Ares::Mouse::Button1
#define ARES_MOUSE_BUTTON_2      ::Ares::Mouse::Button2
#define ARES_MOUSE_BUTTON_3      ::Ares::Mouse::Button3
#define ARES_MOUSE_BUTTON_4      ::Ares::Mouse::Button4
#define ARES_MOUSE_BUTTON_5      ::Ares::Mouse::Button5
#define ARES_MOUSE_BUTTON_6      ::Ares::Mouse::Button6
#define ARES_MOUSE_BUTTON_7      ::Ares::Mouse::Button7
#define ARES_MOUSE_BUTTON_LAST   ::Ares::Mouse::ButtonLast
#define ARES_MOUSE_BUTTON_LEFT   ::Ares::Mouse::ButtonLeft
#define ARES_MOUSE_BUTTON_RIGHT  ::Ares::Mouse::ButtonRight
#define ARES_MOUSE_BUTTON_MIDDLE ::Ares::Mouse::ButtonMiddle

//#define ARES_MOUSE_BUTTON_1         0
//#define ARES_MOUSE_BUTTON_2         1
//#define ARES_MOUSE_BUTTON_3         2
//#define ARES_MOUSE_BUTTON_4         3
//#define ARES_MOUSE_BUTTON_5         4
//#define ARES_MOUSE_BUTTON_6         5
//#define ARES_MOUSE_BUTTON_7         6
//#define ARES_MOUSE_BUTTON_8         7
//#define ARES_MOUSE_BUTTON_LAST      ARES_MOUSE_BUTTON_8
//#define ARES_MOUSE_BUTTON_LEFT      ARES_MOUSE_BUTTON_1
//#define ARES_MOUSE_BUTTON_RIGHT     ARES_MOUSE_BUTTON_2
//#define ARES_MOUSE_BUTTON_MIDDLE    ARES_MOUSE_BUTTON_3
