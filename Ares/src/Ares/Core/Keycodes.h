#pragma once

// from glfw3.h

namespace Ares
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}




//#define ARES_KEY_SPACE              32
//#define ARES_KEY_APOSTROPHE         39  /* ' */
//#define ARES_KEY_COMMA              44  /* , */
//#define ARES_KEY_MINUS              45  /* - */
//#define ARES_KEY_PERIOD             46  /* . */
//#define ARES_KEY_SLASH              47  /* / */
//#define ARES_KEY_0                  48
//#define ARES_KEY_1                  49
//#define ARES_KEY_2                  50
//#define ARES_KEY_3                  51
//#define ARES_KEY_4                  52
//#define ARES_KEY_5                  53
//#define ARES_KEY_6                  54
//#define ARES_KEY_7                  55
//#define ARES_KEY_8                  56
//#define ARES_KEY_9                  57
//#define ARES_KEY_SEMICOLON          59  /* ; */
//#define ARES_KEY_EQUAL              61  /* = */
//#define ARES_KEY_A                  65
//#define ARES_KEY_B                  66
//#define ARES_KEY_C                  67
//#define ARES_KEY_D                  68
//#define ARES_KEY_E                  69
//#define ARES_KEY_F                  70
//#define ARES_KEY_G                  71
//#define ARES_KEY_H                  72
//#define ARES_KEY_I                  73
//#define ARES_KEY_J                  74
//#define ARES_KEY_K                  75
//#define ARES_KEY_L                  76
//#define ARES_KEY_M                  77
//#define ARES_KEY_N                  78
//#define ARES_KEY_O                  79
//#define ARES_KEY_P                  80
//#define ARES_KEY_Q                  81
//#define ARES_KEY_R                  82
//#define ARES_KEY_S                  83
//#define ARES_KEY_T                  84
//#define ARES_KEY_U                  85
//#define ARES_KEY_V                  86
//#define ARES_KEY_W                  87
//#define ARES_KEY_X                  88
//#define ARES_KEY_Y                  89
//#define ARES_KEY_Z                  90
//#define ARES_KEY_LEFT_BRACKET       91  /* [ */
//#define ARES_KEY_BACKSLASH          92  /* \ */
//#define ARES_KEY_RIGHT_BRACKET      93  /* ] */
//#define ARES_KEY_GRAVE_ACCENT       96  /* ` */
//#define ARES_KEY_WORLD_1            161 /* non-US #1 */
//#define ARES_KEY_WORLD_2            162 /* non-US #2 */
#define ARES_KEY_SPACE           ::Ares::Key::Space
#define ARES_KEY_APOSTROPHE      ::Ares::Key::Apostrophe    /* ' */
#define ARES_KEY_COMMA           ::Ares::Key::Comma         /* , */
#define ARES_KEY_MINUS           ::Ares::Key::Minus         /* - */
#define ARES_KEY_PERIOD          ::Ares::Key::Period        /* . */
#define ARES_KEY_SLASH           ::Ares::Key::Slash         /* / */
#define ARES_KEY_0               ::Ares::Key::D0
#define ARES_KEY_1               ::Ares::Key::D1
#define ARES_KEY_2               ::Ares::Key::D2
#define ARES_KEY_3               ::Ares::Key::D3
#define ARES_KEY_4               ::Ares::Key::D4
#define ARES_KEY_5               ::Ares::Key::D5
#define ARES_KEY_6               ::Ares::Key::D6
#define ARES_KEY_7               ::Ares::Key::D7
#define ARES_KEY_8               ::Ares::Key::D8
#define ARES_KEY_9               ::Ares::Key::D9
#define ARES_KEY_SEMICOLON       ::Ares::Key::Semicolon     /* ; */
#define ARES_KEY_EQUAL           ::Ares::Key::Equal         /* = */
#define ARES_KEY_A               ::Ares::Key::A
#define ARES_KEY_B               ::Ares::Key::B
#define ARES_KEY_C               ::Ares::Key::C
#define ARES_KEY_D               ::Ares::Key::D
#define ARES_KEY_E               ::Ares::Key::E
#define ARES_KEY_F               ::Ares::Key::F
#define ARES_KEY_G               ::Ares::Key::G
#define ARES_KEY_H               ::Ares::Key::H
#define ARES_KEY_I               ::Ares::Key::I
#define ARES_KEY_J               ::Ares::Key::J
#define ARES_KEY_K               ::Ares::Key::K
#define ARES_KEY_L               ::Ares::Key::L
#define ARES_KEY_M               ::Ares::Key::M
#define ARES_KEY_N               ::Ares::Key::N
#define ARES_KEY_O               ::Ares::Key::O
#define ARES_KEY_P               ::Ares::Key::P
#define ARES_KEY_Q               ::Ares::Key::Q
#define ARES_KEY_R               ::Ares::Key::R
#define ARES_KEY_S               ::Ares::Key::S
#define ARES_KEY_T               ::Ares::Key::T
#define ARES_KEY_U               ::Ares::Key::U
#define ARES_KEY_V               ::Ares::Key::V
#define ARES_KEY_W               ::Ares::Key::W
#define ARES_KEY_X               ::Ares::Key::X
#define ARES_KEY_Y               ::Ares::Key::Y
#define ARES_KEY_Z               ::Ares::Key::Z
#define ARES_KEY_LEFT_BRACKET    ::Ares::Key::LeftBracket   /* [ */
#define ARES_KEY_BACKSLASH       ::Ares::Key::Backslash     /* \ */
#define ARES_KEY_RIGHT_BRACKET   ::Ares::Key::RightBracket  /* ] */
#define ARES_KEY_GRAVE_ACCENT    ::Ares::Key::GraveAccent   /* ` */
#define ARES_KEY_WORLD_1         ::Ares::Key::World1        /* non-US #1 */
#define ARES_KEY_WORLD_2         ::Ares::Key::World2        /* non-US #2 */

/* Function keys */
//#define ARES_KEY_ESCAPE             256
//#define ARES_KEY_ENTER              257
//#define ARES_KEY_TAB                258
//#define ARES_KEY_BACKSPACE          259
//#define ARES_KEY_INSERT             260
//#define ARES_KEY_DELETE             261
//#define ARES_KEY_RIGHT              262
//#define ARES_KEY_LEFT               263
//#define ARES_KEY_DOWN               264
//#define ARES_KEY_UP                 265
//#define ARES_KEY_PAGE_UP            266
//#define ARES_KEY_PAGE_DOWN          267
//#define ARES_KEY_HOME               268
//#define ARES_KEY_END                269
//#define ARES_KEY_CAPS_LOCK          280
//#define ARES_KEY_SCROLL_LOCK        281
//#define ARES_KEY_NUM_LOCK           282
//#define ARES_KEY_PRINT_SCREEN       283
//#define ARES_KEY_PAUSE              284
//#define ARES_KEY_F1                 290
//#define ARES_KEY_F2                 291
//#define ARES_KEY_F3                 292
//#define ARES_KEY_F4                 293
//#define ARES_KEY_F5                 294
//#define ARES_KEY_F6                 295
//#define ARES_KEY_F7                 296
//#define ARES_KEY_F8                 297
//#define ARES_KEY_F9                 298
//#define ARES_KEY_F10                299
//#define ARES_KEY_F11                300
//#define ARES_KEY_F12                301
//#define ARES_KEY_F13                302
//#define ARES_KEY_F14                303
//#define ARES_KEY_F15                304
//#define ARES_KEY_F16                305
//#define ARES_KEY_F17                306
//#define ARES_KEY_F18                307
//#define ARES_KEY_F19                308
//#define ARES_KEY_F20                309
//#define ARES_KEY_F21                310
//#define ARES_KEY_F22                311
//#define ARES_KEY_F23                312
//#define ARES_KEY_F24                313
//#define ARES_KEY_F25                314
//#define ARES_KEY_KP_0               320
//#define ARES_KEY_KP_1               321
//#define ARES_KEY_KP_2               322
//#define ARES_KEY_KP_3               323
//#define ARES_KEY_KP_4               324
//#define ARES_KEY_KP_5               325
//#define ARES_KEY_KP_6               326
//#define ARES_KEY_KP_7               327
//#define ARES_KEY_KP_8               328
//#define ARES_KEY_KP_9               329
//#define ARES_KEY_KP_DECIMAL         330
//#define ARES_KEY_KP_DIVIDE          331
//#define ARES_KEY_KP_MULTIPLY        332
//#define ARES_KEY_KP_SUBTRACT        333
//#define ARES_KEY_KP_ADD             334
//#define ARES_KEY_KP_ENTER           335
//#define ARES_KEY_KP_EQUAL           336
//#define ARES_KEY_LEFT_SHIFT         340
//#define ARES_KEY_LEFT_CONTROL       341
//#define ARES_KEY_LEFT_ALT           342
//#define ARES_KEY_LEFT_SUPER         343
//#define ARES_KEY_RIGHT_SHIFT        344
//#define ARES_KEY_RIGHT_CONTROL      345
//#define ARES_KEY_RIGHT_ALT          346
//#define ARES_KEY_RIGHT_SUPER        347
//#define ARES_KEY_MENU               348



#define ARES_KEY_ESCAPE          ::Ares::Key::Escape
#define ARES_KEY_ENTER           ::Ares::Key::Enter
#define ARES_KEY_TAB             ::Ares::Key::Tab
#define ARES_KEY_BACKSPACE       ::Ares::Key::Backspace
#define ARES_KEY_INSERT          ::Ares::Key::Insert
#define ARES_KEY_DELETE          ::Ares::Key::Delete
#define ARES_KEY_RIGHT           ::Ares::Key::Right
#define ARES_KEY_LEFT            ::Ares::Key::Left
#define ARES_KEY_DOWN            ::Ares::Key::Down
#define ARES_KEY_UP              ::Ares::Key::Up
#define ARES_KEY_PAGE_UP         ::Ares::Key::PageUp
#define ARES_KEY_PAGE_DOWN       ::Ares::Key::PageDown
#define ARES_KEY_HOME            ::Ares::Key::Home
#define ARES_KEY_END             ::Ares::Key::End
#define ARES_KEY_CAPS_LOCK       ::Ares::Key::CapsLock
#define ARES_KEY_SCROLL_LOCK     ::Ares::Key::ScrollLock
#define ARES_KEY_NUM_LOCK        ::Ares::Key::NumLock
#define ARES_KEY_PRINT_SCREEN    ::Ares::Key::PrintScreen
#define ARES_KEY_PAUSE           ::Ares::Key::Pause
#define ARES_KEY_F1              ::Ares::Key::F1
#define ARES_KEY_F2              ::Ares::Key::F2
#define ARES_KEY_F3              ::Ares::Key::F3
#define ARES_KEY_F4              ::Ares::Key::F4
#define ARES_KEY_F5              ::Ares::Key::F5
#define ARES_KEY_F6              ::Ares::Key::F6
#define ARES_KEY_F7              ::Ares::Key::F7
#define ARES_KEY_F8              ::Ares::Key::F8
#define ARES_KEY_F9              ::Ares::Key::F9
#define ARES_KEY_F10             ::Ares::Key::F10
#define ARES_KEY_F11             ::Ares::Key::F11
#define ARES_KEY_F12             ::Ares::Key::F12
#define ARES_KEY_F13             ::Ares::Key::F13
#define ARES_KEY_F14             ::Ares::Key::F14
#define ARES_KEY_F15             ::Ares::Key::F15
#define ARES_KEY_F16             ::Ares::Key::F16
#define ARES_KEY_F17             ::Ares::Key::F17
#define ARES_KEY_F18             ::Ares::Key::F18
#define ARES_KEY_F19             ::Ares::Key::F19
#define ARES_KEY_F20             ::Ares::Key::F20
#define ARES_KEY_F21             ::Ares::Key::F21
#define ARES_KEY_F22             ::Ares::Key::F22
#define ARES_KEY_F23             ::Ares::Key::F23
#define ARES_KEY_F24             ::Ares::Key::F24
#define ARES_KEY_F25             ::Ares::Key::F25

/* Keypad */
#define ARES_KEY_KP_0            ::Ares::Key::KP0
#define ARES_KEY_KP_1            ::Ares::Key::KP1
#define ARES_KEY_KP_2            ::Ares::Key::KP2
#define ARES_KEY_KP_3            ::Ares::Key::KP3
#define ARES_KEY_KP_4            ::Ares::Key::KP4
#define ARES_KEY_KP_5            ::Ares::Key::KP5
#define ARES_KEY_KP_6            ::Ares::Key::KP6
#define ARES_KEY_KP_7            ::Ares::Key::KP7
#define ARES_KEY_KP_8            ::Ares::Key::KP8
#define ARES_KEY_KP_9            ::Ares::Key::KP9
#define ARES_KEY_KP_DECIMAL      ::Ares::Key::KPDecimal
#define ARES_KEY_KP_DIVIDE       ::Ares::Key::KPDivide
#define ARES_KEY_KP_MULTIPLY     ::Ares::Key::KPMultiply
#define ARES_KEY_KP_SUBTRACT     ::Ares::Key::KPSubtract
#define ARES_KEY_KP_ADD          ::Ares::Key::KPAdd
#define ARES_KEY_KP_ENTER        ::Ares::Key::KPEnter
#define ARES_KEY_KP_EQUAL        ::Ares::Key::KPEqual

#define ARES_KEY_LEFT_SHIFT      ::Ares::Key::LeftShift
#define ARES_KEY_LEFT_CONTROL    ::Ares::Key::LeftControl
#define ARES_KEY_LEFT_ALT        ::Ares::Key::LeftAlt
#define ARES_KEY_LEFT_SUPER      ::Ares::Key::LeftSuper
#define ARES_KEY_RIGHT_SHIFT     ::Ares::Key::RightShift
#define ARES_KEY_RIGHT_CONTROL   ::Ares::Key::RightControl
#define ARES_KEY_RIGHT_ALT       ::Ares::Key::RightAlt
#define ARES_KEY_RIGHT_SUPER     ::Ares::Key::RightSuper
#define ARES_KEY_MENU            ::Ares::Key::Menu