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

		static void Toggle(const std::string& name, bool& value);
		
		static void Float(const std::string& name, float& value);
		static void FloatSlider(const std::string& name, float& value, float min = -1.0f, float max = 1.0f);
		
		static void Int(const std::string& name, int& value);
		static void IntSlider(const std::string& name, int& value, int min = -1, int max = 1);
		
		static void Vec2(const std::string& name, glm::vec2& value);
		static void Vec2Slider(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f);
		
		static void Vec3(const std::string& name, glm::vec3& value);
		static void Vec3Slider(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f);
		
		static void Vec4(const std::string& name, glm::vec4& value);
		static void Vec4Slider(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f);

		static void Color3(const std::string& name, glm::vec3& value);
		static void Color4(const std::string& name, glm::vec4& value);

	};
}