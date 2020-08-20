#pragma once


namespace Ares
{

// from glfw3.h
#define _ARES_KEY_SPACE           32
#define _ARES_KEY_APOSTROPHE      39
#define _ARES_KEY_COMMA           44
#define _ARES_KEY_MINUS           45
#define _ARES_KEY_PERIOD          46
#define _ARES_KEY_SLASH           47

#define _ARES_KEY_0               48
#define _ARES_KEY_1               49
#define _ARES_KEY_2               50
#define _ARES_KEY_3               51
#define _ARES_KEY_4               52
#define _ARES_KEY_5               53
#define _ARES_KEY_6               54
#define _ARES_KEY_7               55
#define _ARES_KEY_8               56
#define _ARES_KEY_9               57

#define _ARES_KEY_SEMICOLON       59
#define _ARES_KEY_EQUAL           61
#define _ARES_KEY_A               65
#define _ARES_KEY_B               66
#define _ARES_KEY_C               67
#define _ARES_KEY_D               68
#define _ARES_KEY_E               69
#define _ARES_KEY_F               70
#define _ARES_KEY_G               71
#define _ARES_KEY_H               72
#define _ARES_KEY_I               73
#define _ARES_KEY_J               74
#define _ARES_KEY_K               75
#define _ARES_KEY_L               76
#define _ARES_KEY_M               77
#define _ARES_KEY_N               78
#define _ARES_KEY_O               79
#define _ARES_KEY_P               80
#define _ARES_KEY_Q               81
#define _ARES_KEY_R               82
#define _ARES_KEY_S               83
#define _ARES_KEY_T               84
#define _ARES_KEY_U               85
#define _ARES_KEY_V               86
#define _ARES_KEY_W               87
#define _ARES_KEY_X               88
#define _ARES_KEY_Y               89
#define _ARES_KEY_Z               90

#define _ARES_KEY_LEFT_BRACKET    91
#define _ARES_KEY_BACKSLASH       92
#define _ARES_KEY_RIGHT_BRACKET   93
#define _ARES_KEY_GRAVE_ACCENT    96
#define _ARES_KEY_WORLD_1         161
#define _ARES_KEY_WORLD_2         162

/* Function keys */
#define _ARES_KEY_ESCAPE          256
#define _ARES_KEY_ENTER           257
#define _ARES_KEY_TAB             258
#define _ARES_KEY_BACKSPACE       259
#define _ARES_KEY_INSERT          260
#define _ARES_KEY_DELETE          261
#define _ARES_KEY_RIGHT           262
#define _ARES_KEY_LEFT            263
#define _ARES_KEY_DOWN            264
#define _ARES_KEY_UP              265
#define _ARES_KEY_PAGE_UP         266
#define _ARES_KEY_PAGE_DOWN       267
#define _ARES_KEY_HOME            268
#define _ARES_KEY_END             269
#define _ARES_KEY_CAPS_LOCK       280
#define _ARES_KEY_SCROLL_LOCK     281
#define _ARES_KEY_NUM_LOCK        282
#define _ARES_KEY_PRINT_SCREEN    283
#define _ARES_KEY_PAUSE           284

#define _ARES_KEY_F1              290
#define _ARES_KEY_F2              291
#define _ARES_KEY_F3              292
#define _ARES_KEY_F4              293
#define _ARES_KEY_F5              294
#define _ARES_KEY_F6              295
#define _ARES_KEY_F7              296
#define _ARES_KEY_F8              297
#define _ARES_KEY_F9              298
#define _ARES_KEY_F10             299
#define _ARES_KEY_F11             300
#define _ARES_KEY_F12             301
#define _ARES_KEY_F13             302
#define _ARES_KEY_F14             303
#define _ARES_KEY_F15             304
#define _ARES_KEY_F16             305
#define _ARES_KEY_F17             306
#define _ARES_KEY_F18             307
#define _ARES_KEY_F19             308
#define _ARES_KEY_F20             309
#define _ARES_KEY_F21             310
#define _ARES_KEY_F22             311
#define _ARES_KEY_F23             312
#define _ARES_KEY_F24             313
#define _ARES_KEY_F25             314

/* Keypad */
#define _ARES_KEY_KP_0            320
#define _ARES_KEY_KP_1            321
#define _ARES_KEY_KP_2            322
#define _ARES_KEY_KP_3            323
#define _ARES_KEY_KP_4            324
#define _ARES_KEY_KP_5            325
#define _ARES_KEY_KP_6            326
#define _ARES_KEY_KP_7            327
#define _ARES_KEY_KP_8            328
#define _ARES_KEY_KP_9            329

#define _ARES_KEY_KP_DECIMAL      330
#define _ARES_KEY_KP_DIVIDE       331
#define _ARES_KEY_KP_MULTIPLY     332
#define _ARES_KEY_KP_SUBTRACT     333
#define _ARES_KEY_KP_ADD          334
#define _ARES_KEY_KP_ENTER        335
#define _ARES_KEY_KP_EQUAL        336

#define _ARES_KEY_LEFT_SHIFT      340
#define _ARES_KEY_LEFT_CONTROL    341
#define _ARES_KEY_LEFT_ALT        342
#define _ARES_KEY_LEFT_SUPER      343
#define _ARES_KEY_RIGHT_SHIFT     344
#define _ARES_KEY_RIGHT_CONTROL   345
#define _ARES_KEY_RIGHT_ALT       346
#define _ARES_KEY_RIGHT_SUPER     347
#define _ARES_KEY_MENU            348







	typedef enum class KeyCode : uint16_t
	{
		Space			= _ARES_KEY_SPACE           ,
		Apostrophe		= _ARES_KEY_APOSTROPHE      , 
		Comma			= _ARES_KEY_COMMA           , 
		Minus			= _ARES_KEY_MINUS           , 
		Period			= _ARES_KEY_PERIOD          , 
		Slash			= _ARES_KEY_SLASH           , 
		D0				= _ARES_KEY_0               , 
		D1				= _ARES_KEY_1               , 
		D2				= _ARES_KEY_2               , 
		D3				= _ARES_KEY_3               , 
		D4				= _ARES_KEY_4               , 
		D5				= _ARES_KEY_5               , 
		D6				= _ARES_KEY_6               , 
		D7				= _ARES_KEY_7               , 
		D8				= _ARES_KEY_8               , 
		D9				= _ARES_KEY_9               , 
		Semicolon		= _ARES_KEY_SEMICOLON       , 
		Equal			= _ARES_KEY_EQUAL           , 
		A				= _ARES_KEY_A               ,
		B				= _ARES_KEY_B               ,
		C				= _ARES_KEY_C               ,
		D				= _ARES_KEY_D               ,
		E				= _ARES_KEY_E               ,
		F				= _ARES_KEY_F               ,
		G				= _ARES_KEY_G               ,
		H				= _ARES_KEY_H               ,
		I				= _ARES_KEY_I               ,
		J				= _ARES_KEY_J               ,
		K				= _ARES_KEY_K               ,
		L				= _ARES_KEY_L               ,
		M				= _ARES_KEY_M               ,
		N				= _ARES_KEY_N               ,
		O				= _ARES_KEY_O               ,
		P				= _ARES_KEY_P               ,
		Q				= _ARES_KEY_Q               ,
		R				= _ARES_KEY_R               ,
		S				= _ARES_KEY_S               ,
		T				= _ARES_KEY_T               ,
		U				= _ARES_KEY_U               ,
		V				= _ARES_KEY_V               ,
		W				= _ARES_KEY_W               ,
		X				= _ARES_KEY_X               ,
		Y				= _ARES_KEY_Y               ,
		Z				= _ARES_KEY_Z               ,
		LeftBracket		= _ARES_KEY_LEFT_BRACKET    ,  
		Backslash		= _ARES_KEY_BACKSLASH       ,  
		RightBracket	= _ARES_KEY_RIGHT_BRACKET   ,  
		GraveAccent		= _ARES_KEY_GRAVE_ACCENT    ,  
		World2			= _ARES_KEY_WORLD_1         , 
		World1			= _ARES_KEY_WORLD_2         , 
		Escape			= _ARES_KEY_ESCAPE          ,
		Enter			= _ARES_KEY_ENTER           ,
		Tab				= _ARES_KEY_TAB             ,
		Backspace		= _ARES_KEY_BACKSPACE       ,
		Insert			= _ARES_KEY_INSERT          ,
		Delete			= _ARES_KEY_DELETE          ,
		Right			= _ARES_KEY_RIGHT           ,
		Left			= _ARES_KEY_LEFT            ,
		Down			= _ARES_KEY_DOWN            ,
		Up				= _ARES_KEY_UP              ,
		PageUp			= _ARES_KEY_PAGE_UP         ,
		PageDown		= _ARES_KEY_PAGE_DOWN       ,
		Home			= _ARES_KEY_HOME            ,
		End				= _ARES_KEY_END             ,
		CapsLock		= _ARES_KEY_CAPS_LOCK       ,
		ScrollLock		= _ARES_KEY_SCROLL_LOCK     ,
		NumLock			= _ARES_KEY_NUM_LOCK        ,
		PrintScreen		= _ARES_KEY_PRINT_SCREEN    ,
		Pause			= _ARES_KEY_PAUSE           ,
		F1				= _ARES_KEY_F1              ,
		F2				= _ARES_KEY_F2              ,
		F3				= _ARES_KEY_F3              ,
		F4				= _ARES_KEY_F4              ,
		F5				= _ARES_KEY_F5              ,
		F6				= _ARES_KEY_F6              ,
		F7				= _ARES_KEY_F7              ,
		F8				= _ARES_KEY_F8              ,
		F9				= _ARES_KEY_F9              ,
		F10				= _ARES_KEY_F10             ,
		F11				= _ARES_KEY_F11             ,
		F12				= _ARES_KEY_F12             ,
		F13				= _ARES_KEY_F13             ,
		F14				= _ARES_KEY_F14             ,
		F15				= _ARES_KEY_F15             ,
		F16				= _ARES_KEY_F16             ,
		F17				= _ARES_KEY_F17             ,
		F18				= _ARES_KEY_F18             ,
		F19				= _ARES_KEY_F19             ,
		F20				= _ARES_KEY_F20             ,
		F21				= _ARES_KEY_F21             ,
		F22				= _ARES_KEY_F22             ,
		F23				= _ARES_KEY_F23             ,
		F24				= _ARES_KEY_F24             ,
		F25				= _ARES_KEY_F25             ,
		KP0				= _ARES_KEY_KP_0            ,
		KP1				= _ARES_KEY_KP_1            ,
		KP2				= _ARES_KEY_KP_2            ,
		KP3				= _ARES_KEY_KP_3            ,
		KP4				= _ARES_KEY_KP_4            ,
		KP5				= _ARES_KEY_KP_5            ,
		KP6				= _ARES_KEY_KP_6            ,
		KP7				= _ARES_KEY_KP_7            ,
		KP8				= _ARES_KEY_KP_8            ,
		KP9				= _ARES_KEY_KP_9            ,
		KPDecimal		= _ARES_KEY_KP_DECIMAL      ,
		KPDivide		= _ARES_KEY_KP_DIVIDE       ,
		KPMultiply		= _ARES_KEY_KP_MULTIPLY     ,
		KPSubtract		= _ARES_KEY_KP_SUBTRACT     ,
		KPAdd			= _ARES_KEY_KP_ADD          ,
		KPEnter			= _ARES_KEY_KP_ENTER        ,
		KPEqual			= _ARES_KEY_KP_EQUAL        ,
		LeftShift		= _ARES_KEY_LEFT_SHIFT      ,
		LeftControl		= _ARES_KEY_LEFT_CONTROL    ,
		LeftAlt			= _ARES_KEY_LEFT_ALT        ,
		LeftSuper		= _ARES_KEY_LEFT_SUPER      ,
		RightShift		= _ARES_KEY_RIGHT_SHIFT     ,
		RightControl	= _ARES_KEY_RIGHT_CONTROL   ,
		RightAlt		= _ARES_KEY_RIGHT_ALT       ,
		RightSuper		= _ARES_KEY_RIGHT_SUPER     ,
		Menu			= _ARES_KEY_MENU            
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}
