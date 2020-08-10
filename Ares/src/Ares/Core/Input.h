#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Keycodes.h"
#include "Ares/Core/MouseButtonCodes.h"

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
	public:
		static bool IsKeyPressed(const KeyCode key);
		static bool IsMouseButtonPressed(const MouseButtonCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

		static bool IsInputPressed(const std::string& name)
		{
			if (s_Mapping.count(name) != 0)
				return IsInputPressed(s_Mapping[name]);

			ARES_CORE_WARN("The input {0} is not mapped!", name);
			return false;
		}

		inline static bool IsInputPressed(InputKey inputKey)
		{
			if (std::holds_alternative<KeyCode>(inputKey.value))
				return IsKeyPressed(std::get<KeyCode>(inputKey.value));

			if (std::holds_alternative<MouseButtonCode>(inputKey.value))
				return IsMouseButtonPressed(std::get<MouseButtonCode>(inputKey.value));

			// control should never fall down here;
			ARES_CORE_WARN("Unknown Input type!");
			return false;
		}

		inline static std::unordered_map<std::string, InputKey> s_Mapping;

		/* Map a Key/Mouse Button to an InputKey, and overwrite the old value if existed. */
		inline static void Remap(const std::string& name, InputKey inputKey) { s_Mapping[name] = inputKey; }
		
		inline static void Remap(const std::string& name, KeyCode key) { s_Mapping[name] = { key }; }
		inline static void Remap(const std::string& name, MouseButtonCode button) { s_Mapping[name] = { button }; }

	};

	
}