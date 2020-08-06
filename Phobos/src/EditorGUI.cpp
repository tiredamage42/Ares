
#include "EditorGUI.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
//#include "imgui/imgui_internal.h"

namespace Ares
{
	void EditorGUI::ShowTooltip(const char* text)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(text);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

#define START_FIELD(name) \
	ImGui::Text(name.c_str()); \
	ImGui::NextColumn(); \
	ImGui::PushItemWidth(-1); \
	std::string id = "##" + name;

#define END_FIELD() \
	ImGui::PopItemWidth(); \
	ImGui::NextColumn();



	void EditorGUI::Toggle(const std::string& name, bool& value)
	{
		START_FIELD(name)
		ImGui::Checkbox(id.c_str(), &value);
		END_FIELD()
	}
	void EditorGUI::Float(const std::string& name, float& value)
	{
		START_FIELD(name)
		ImGui::DragFloat(id.c_str(), &value);
		END_FIELD()
	}
	void EditorGUI::FloatSlider(const std::string& name, float& value, float min, float max)
	{
		START_FIELD(name)
		ImGui::SliderFloat(id.c_str(), &value, min, max);
		END_FIELD()
	}
	void EditorGUI::Int(const std::string& name, int& value)
	{
		START_FIELD(name)
		ImGui::DragInt(id.c_str(), &value);
		END_FIELD()
	}
	void EditorGUI::IntSlider(const std::string& name, int& value, int min, int max)
	{
		START_FIELD(name)
		ImGui::SliderInt(id.c_str(), &value, min, max);
		END_FIELD()
	}
	void EditorGUI::Vec2(const std::string& name, glm::vec2& value)
	{
		START_FIELD(name)
		ImGui::DragFloat2(id.c_str(), glm::value_ptr(value));
		END_FIELD()
	}
	void EditorGUI::Vec2Slider(const std::string& name, glm::vec2& value, float min, float max)
	{
		START_FIELD(name)
		ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max);
		END_FIELD()
	}
	void EditorGUI::Vec3(const std::string& name, glm::vec3& value)
	{
		START_FIELD(name)
		ImGui::DragFloat3(id.c_str(), glm::value_ptr(value));
		END_FIELD()
	}
	void EditorGUI::Vec3Slider(const std::string& name, glm::vec3& value, float min, float max)
	{
		START_FIELD(name)
		ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);
		END_FIELD()
	}
	void EditorGUI::Vec4(const std::string& name, glm::vec4& value)
	{
		START_FIELD(name)
		ImGui::DragFloat4(id.c_str(), glm::value_ptr(value));
		END_FIELD()
	}
	void EditorGUI::Vec4Slider(const std::string& name, glm::vec4& value, float min, float max)
	{
		START_FIELD(name)
		ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);
		END_FIELD()
	}
	void EditorGUI::Color3(const std::string& name, glm::vec3& value)
	{
		START_FIELD(name)
		ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value));// , ImGuiColorEditFlags_NoInputs);
		END_FIELD()
	}
	void EditorGUI::Color4(const std::string& name, glm::vec4& value)
	{
		START_FIELD(name)
		ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
		END_FIELD()
	}
}