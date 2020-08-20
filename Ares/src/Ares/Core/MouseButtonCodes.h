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






	static uint32_t MouseCode2Index(MouseButtonCode code)
	{
		switch (code)
		{
			case MouseButtonCode::Button0: return 0;
			case MouseButtonCode::Button1: return 1;
			case MouseButtonCode::Button2: return 2;
			case MouseButtonCode::Button3: return 3;
			case MouseButtonCode::Button4: return 4;
			case MouseButtonCode::Button5: return 5;
			case MouseButtonCode::Button6: return 6;
			case MouseButtonCode::Button7: return 7;
			
		default:
			break;
		}
		return 0;
	}

	static const MouseButtonCode ALL_MOUSECODES[] = {
		MouseButtonCode::Button0,
		MouseButtonCode::Button1,
		MouseButtonCode::Button2,
		MouseButtonCode::Button3,
		MouseButtonCode::Button4,
		MouseButtonCode::Button5,
		MouseButtonCode::Button6,
		MouseButtonCode::Button7,
	};

	static const size_t MOUSECODES_COUNT = sizeof(ALL_MOUSECODES) / sizeof(ALL_MOUSECODES[0]);		
}
