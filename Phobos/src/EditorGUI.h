#pragma once

#include <glm/glm.hpp>

#include <string>
namespace Ares
{

	// ImGui UI helpers
	class EditorGUI
	{
	public:
		static void ShowTooltip(const char* text);

		static bool Toggle(const std::string& name, bool& value);
		
		static bool Float(const std::string& name, float& value);
		static bool FloatSlider(const std::string& name, float& value, float min = -1.0f, float max = 1.0f);
		
		static bool Int(const std::string& name, int& value);
		static bool IntSlider(const std::string& name, int& value, int min = -1, int max = 1);
		
		static bool Vec2(const std::string& name, glm::vec2& value);
		static bool Vec2Slider(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f);
		
		static bool Vec3(const std::string& name, glm::vec3& value);
		static bool Vec3Slider(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f);
		
		static bool Vec4(const std::string& name, glm::vec4& value);
		static bool Vec4Slider(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f);

		static bool Color3(const std::string& name, glm::vec3& value);
		static bool Color4(const std::string& name, glm::vec4& value);

	};
}