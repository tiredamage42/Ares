#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Keycodes.h"
#include "Ares/Core/MouseButtonCodes.h"
#include "Ares/Math/Math.h"
#include <variant>
namespace Ares {


	/* Holds a single input, either a KeyCode or a MouseCode. */
	struct InputKey
	{
		std::variant<KeyCode, MouseButtonCode> value;

		InputKey() = default;
		InputKey(KeyCode key) { value = key; }
		InputKey(MouseButtonCode button) { value = button; }
	};

	class Input
	{
		friend class Application;

	public:
#define _INPUT_GETS(TYPE, CODE, ARRAY) \
	static bool Get##TYPE##Down(CODE code) \
	{ \
		uint32_t i = CODE##2Index(code); \
		return ARRAY[curFrame][i] && !ARRAY[(curFrame + 1) % 2][i]; \
	} \
	static bool Get##TYPE##Up(CODE code) \
	{ \
		uint32_t i = CODE##2Index(code); \
		return !ARRAY[curFrame][i] && ARRAY[(curFrame + 1) % 2][i]; \
	} \
	static bool Get##TYPE##(CODE code) \
	{ \
		return ARRAY[curFrame][CODE##2Index(code)]; \
	}


		_INPUT_GETS(Key, KeyCode, s_KeyPressed)
		_INPUT_GETS(MouseButton, MouseButtonCode, s_MousePressed)

			/*
		static bool GetKeyDown(KeyCode code)
		{
			uint32_t i = KeyCode2Index(code);
			return s_KeyPressed[curFrame][i] && !s_KeyPressed[(curFrame + 1) % 2][i];
		}
		static bool GetKeyUp(KeyCode code)
		{
			uint32_t i = KeyCode2Index(code);
			return !s_KeyPressed[curFrame][i] && s_KeyPressed[(curFrame + 1) % 2][i];
		}
		static bool GetKey(KeyCode code)
		{
			return s_KeyPressed[curFrame][KeyCode2Index(code)];
		}


		static bool GetMouseButtonDown(MouseButtonCode code)
		{
			uint32_t i = MouseCode2Index(code);
			return s_MousePressed[curFrame][i] && !s_MousePressed[(curFrame + 1) % 2][i];
		}
		static bool GetMouseButtonUp(MouseButtonCode code)
		{
			uint32_t i = MouseCode2Index(code);
			return !s_MousePressed[curFrame][i] && s_MousePressed[(curFrame + 1) % 2][i];
		}
		static bool GetMouseButton(MouseButtonCode code)
		{
			return s_MousePressed[curFrame][MouseCode2Index(code)];
		}
			*/
#define _BUTTON_GETS(ACTION) \
	inline static bool GetButton##ACTION(const std::string& name) \
	{ \
		if (s_Mapping.count(name) != 0) return GetButton##ACTION(s_Mapping[name]); \
		ARES_CORE_WARN("The input {0} is not mapped!", name); \
		return false; \
	} \
	inline static bool GetButton##ACTION(InputKey inputKey) \
	{ \
		if (std::holds_alternative<KeyCode>(inputKey.value)) return GetKey##ACTION(std::get<KeyCode>(inputKey.value)); \
		if (std::holds_alternative<MouseButtonCode>(inputKey.value)) return GetMouseButton##ACTION(std::get<MouseButtonCode>(inputKey.value)); \
		ARES_CORE_WARN("Unknown Input type!"); \
		return false; \
	}

	_BUTTON_GETS()
	_BUTTON_GETS(Up)
	_BUTTON_GETS(Down)

			/*
		static bool GetButton(const std::string& name)
		{
			if (s_Mapping.count(name) != 0)
				return GetButton(s_Mapping[name]);

			ARES_CORE_WARN("The input {0} is not mapped!", name);
			return false;
		}

		inline static bool GetButton(InputKey inputKey)
		{
			if (std::holds_alternative<KeyCode>(inputKey.value))
				return GetKey(std::get<KeyCode>(inputKey.value));

			if (std::holds_alternative<MouseButtonCode>(inputKey.value))
				return GetMouseButton(std::get<MouseButtonCode>(inputKey.value));

			// control should never fall down here;
			ARES_CORE_WARN("Unknown Input type!");
			return false;
		}


		static bool GetButtonDown(const std::string& name)
		{
			if (s_Mapping.count(name) != 0)
				return GetButtonDown(s_Mapping[name]);

			ARES_CORE_WARN("The input {0} is not mapped!", name);
			return false;
		}

		inline static bool GetButtonDown(InputKey inputKey)
		{
			if (std::holds_alternative<KeyCode>(inputKey.value))
				return GetKeyDown(std::get<KeyCode>(inputKey.value));

			if (std::holds_alternative<MouseButtonCode>(inputKey.value))
				return GetMouseButtonDown(std::get<MouseButtonCode>(inputKey.value));

			// control should never fall down here;
			ARES_CORE_WARN("Unknown Input type!");
			return false;
		}


		static bool GetButtonUp(const std::string& name)
		{
			if (s_Mapping.count(name) != 0)
				return GetButtonUp(s_Mapping[name]);

			ARES_CORE_WARN("The input {0} is not mapped!", name);
			return false;
		}

		inline static bool GetButtonUp(InputKey inputKey)
		{
			if (std::holds_alternative<KeyCode>(inputKey.value))
				return GetKeyUp(std::get<KeyCode>(inputKey.value));

			if (std::holds_alternative<MouseButtonCode>(inputKey.value))
				return GetMouseButtonUp(std::get<MouseButtonCode>(inputKey.value));

			// control should never fall down here;
			ARES_CORE_WARN("Unknown Input type!");
			return false;
		}
			*/

	protected:
		inline static bool s_KeyPressed[2][KEYCODES_COUNT];
		inline static bool s_MousePressed[2][MOUSECODES_COUNT];

		inline static uint32_t curFrame = 0;
		static void InitializeInput()
		{
			curFrame = 0;
			for (uint32_t i = 0; i < KEYCODES_COUNT; i++)
			{
				s_KeyPressed[0][i] = false;
				s_KeyPressed[1][i] = false;
			}
			for (uint32_t i = 0; i < MOUSECODES_COUNT; i++)
			{
				s_MousePressed[0][i] = false;
				s_MousePressed[1][i] = false;
			}
		}
		static void UpdateInputPolls();

	public:
		static Vector2 GetMousePosition();
		inline static std::unordered_map<std::string, InputKey> s_Mapping;
		/* Map a Key/Mouse Button to an InputKey, and overwrite the old value if existed. */
		inline static void Remap(const std::string& name, InputKey inputKey) { s_Mapping[name] = inputKey; }
		inline static void Remap(const std::string& name, KeyCode key) { s_Mapping[name] = { key }; }
		inline static void Remap(const std::string& name, MouseButtonCode button) { s_Mapping[name] = { button }; }
	};
}