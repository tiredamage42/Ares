#include "PhobosPCH.h"
#include "EditorGUI.h"
#include "EditorUtility.h"

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



	static int s_UIContextID = 0;
	static uint32_t s_Counter = 0;
	
	static void PushID()
	{
		ImGui::PushID(s_UIContextID++);
		s_Counter = 0;
	}

	static void PopID()
	{
		ImGui::PopID();
		s_UIContextID--;
	}

	void EditorGUI::BeginPropertyGrid()
	{
		PushID();
		ImGui::Columns(2);
	}

	void EditorGUI::EndPropertyGrid()
	{
		ImGui::Columns(1);
		PopID();
	}


#define _START_FIELD(DRAWMETHOD, VALTYPE, OLDVAL) \
	VALTYPE v = value; \
	ImGui::Text(name.c_str()); \
	ImGui::NextColumn(); \
	ImGui::PushItemWidth(-1); \
	std::string id = "##" + name; \
	bool result = DRAWMETHOD; \
	ImGui::PopItemWidth(); \
	ImGui::NextColumn(); \
	if (ImGui::IsItemActivated()) \
	{ \
		OLDVAL = v; \
	} \
	if (ImGui::IsItemDeactivatedAfterEdit()) \
	{ \
		VALTYPE ov = OLDVAL; \
		VALTYPE nv = value;

#define EDITOR_FIELD(DRAWMETHOD, VALTYPE, OLDVAL) \
	_START_FIELD(DRAWMETHOD, VALTYPE, OLDVAL) \
		EditorUtility::AddToUndoStack({ \
			[&value, ov]() { value = ov; }, \
			[&value, nv]() { value = nv; } \
		}); \
	} \
	return result;

#define EDITOR_FIELD_CUSTOM_UNDO_REDO(DRAWMETHOD, VALTYPE, OLDVAL) \
	_START_FIELD(DRAWMETHOD, VALTYPE, OLDVAL) \
		EditorUtility::AddToUndoStack({ \
			[CUSTOM_UNDO, ov]() { CUSTOM_UNDO(ov); }, \
			[CUSTOM_UNDO, nv]() { CUSTOM_UNDO(nv); } \
		}); \
	} \
	return result;


	static std::string oldValues;
	static bool oldValueb;
	static float oldValuef;
	static int oldValuei;
	static Vector2 oldValue2;
	static Vector3 oldValue3;
	static Vector4 oldValue4;
	static ImVec4 oldValueim4;

	/*
	bool EditorGUI::StringField(PARAMS_CUSTOM_UNDO_REDO(std::string))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::InputText(id.c_str(), &value), std::string, oldValues);
	}
	bool EditorGUI::StringField(PARAMS(std::string))
	{
		EDITOR_FIELD(ImGui::InputText(id.c_str(), &value), std::string, oldValues);
	}
	*/

	/*
#define FLAGS_FIELD(UNDO_REDO) \
	ImGui::Text(name.c_str()); \
	ImGui::NextColumn(); \
	ImGui::PushItemWidth(-1); \
	uint32_t count = sizeof(allFlags) / sizeof(allFlags[0]); \
	std::string preview = ""; \
	for (uint32_t i = 0; i < count; i++) \
		if ((uint32_t)allFlags[i] & value) \
			preview += allFlagNames[i] + ","; \
	bool edited = false; \
	if (ImGui::BeginCombo("##" + name.c_str(), preview.c_str())) \
	{ \
		for (uint32_t i = 0; i < count; i++) \
		{ \
			bool hasFlag = (uint32_t)allFlags[i] & value; \
			if (ImGui::Selectable(allFlagNames[i], &hasFlag, ImGuiItemFlags_SelectableDontClosePopup)) \
			{ \
				edited = true; \
				uint32_t ov = value; \
				if (hasFlag) \
					value |= (uint32_t)allFlags[i]; \
				else \
					value &= ~(uint32_t)allFlags[i]; \
				uint32_t nv = value; \
				UNDO_REDO \
			} \
		} \
		ImGui::EndCombo(); \
	} \
	ImGui::PopItemWidth(); \
	ImGui::NextColumn(); \
	return edited;



	template<typename T>
	bool EditorGUI::FlagsField(PARAMS(uint32_t), const T allFlags[], const char* allFlagNames[])
	{
		FLAGS_FIELD(
			EditorUtility::AddToUndoStack({
				[&value, ov]() { value = ov; },
				[&value, nv]() { value = nv; }
			});
		)
	}
	template<typename T>
	bool EditorGUI::FlagsField(PARAMS_CUSTOM_UNDO_REDO(uint32_t), const T allFlags[], const char* allFlagNames[])
	{
		FLAGS_FIELD(
			EditorUtility::AddToUndoStack({
				[CUSTOM_UNDO, ov]() { CUSTOM_UNDO(ov); }, 
				[CUSTOM_UNDO, nv]() { CUSTOM_UNDO(nv); } 
			});
		)
	}

#define ENUM_FIELD(UNDO_REDO) \
	ImGui::Text(name.c_str()); \
	ImGui::NextColumn(); \
	ImGui::PushItemWidth(-1); \
	uint32_t count = sizeof(allEnums) / sizeof(allEnums[0]); \
	uint32_t preview = 0; \
	for (uint32_t i = 0; i < count; i++) \
	{ \
		if (value == allEnums[i]) \
		{ \
			preview = i; \
			break; \
		} \
	} \
	bool edited = false; \
	if (ImGui::BeginCombo("##" + name.c_str(), allEnumNames[preview])) \
	{ \
		for (uint32_t i = 0; i < count; i++) \
		{ \
			bool isSelected = preview == i; \
			if (ImGui::Selectable(allFlagNames[i], &isSelected, ImGuiItemFlags_SelectableDontClosePopup)) \
			{ \
				if (preview != i) \
				{ \
					edited = true; \
					T ov = value; \
					value = allEnums[i]; \
					T nv = value; \
					UNDO_REDO \
				} \
			} \
			if (preview == i) \
				ImGui::SetItemDefaultFocus(); \
		} \
		ImGui::EndCombo(); \
	} \
	ImGui::PopItemWidth(); \
	ImGui::NextColumn(); \
	return edited;


	template<typename T>
	bool EditorGUI::EnumField(PARAMS_CUSTOM_UNDO_REDO(T), const T allEnums[], const char* allEnumNames[])
	{
		ENUM_FIELD(
			EditorUtility::AddToUndoStack({
				[&value, ov]() { value = ov; },
				[&value, nv]() { value = nv; }
			});
		)
	}
	template<typename T>
	bool EditorGUI::EnumField(PARAMS(T), const T allEnums[], const char* allEnumNames[])
	{
		ENUM_FIELD(
			EditorUtility::AddToUndoStack({
				[CUSTOM_UNDO, ov]() { CUSTOM_UNDO(ov); },
				[CUSTOM_UNDO, nv]() { CUSTOM_UNDO(nv); }
			});
		)
	}
	*/


	/*
	
	const char* projTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProj = projTypeStrings[(int)cc->Camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProj))
				{
					for (int type = 0; type < 2; type++)
					{
						bool is_selected = (currentProj == projTypeStrings[type]);
						if (ImGui::Selectable(projTypeStrings[type], is_selected, ImGuiItemFlags_SelectableDontClosePopup))
						{
							currentProj = projTypeStrings[type];
							cc->Camera.SetProjectionType((SceneCamera::ProjectionType)type);
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

	
	*/



	bool EditorGUI::ToggleField(PARAMS_CUSTOM_UNDO_REDO(bool))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::Checkbox(id.c_str(), &value), bool, oldValueb);
	}
	bool EditorGUI::ToggleField(PARAMS(bool))
	{
		EDITOR_FIELD(ImGui::Checkbox(id.c_str(), &value), bool, oldValueb);
	}
		 
	bool EditorGUI::FloatField(PARAMS_CUSTOM_UNDO_REDO(float))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::DragFloat(id.c_str(), &value), float, oldValuef);
	}
	bool EditorGUI::FloatSliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(float))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::SliderFloat(id.c_str(), &value, min, max), float, oldValuef);
	}
	bool EditorGUI::FloatField(PARAMS(float))
	{
		EDITOR_FIELD(ImGui::DragFloat(id.c_str(), &value), float, oldValuef);
	}
	bool EditorGUI::FloatSliderField(PARAMS_SLIDER(float))
	{
		EDITOR_FIELD(ImGui::SliderFloat(id.c_str(), &value, min, max), float, oldValuef);
	}
		 
	bool EditorGUI::IntField(PARAMS_CUSTOM_UNDO_REDO(int))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::DragInt(id.c_str(), &value), int, oldValuei);
	}
	bool EditorGUI::IntSliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(int))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::SliderInt(id.c_str(), &value, (int)min, (int)max), int, oldValuei);
	}
	bool EditorGUI::IntField(PARAMS(int))
	{
		EDITOR_FIELD(ImGui::DragInt(id.c_str(), &value), int, oldValuei);
	}
	bool EditorGUI::IntSliderField(PARAMS_SLIDER(int))
	{
		EDITOR_FIELD(ImGui::SliderInt(id.c_str(), &value, (int)min, (int)max), int, oldValuei);
	}
		 
	bool EditorGUI::Vec2Field(PARAMS_CUSTOM_UNDO_REDO(Vector2))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::DragFloat2(id.c_str(), glm::value_ptr(value)), Vector2, oldValue2);
	}
	bool EditorGUI::Vec2SliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(Vector2))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max), Vector2, oldValue2);
	}
	bool EditorGUI::Vec2Field(PARAMS(Vector2))
	{
		EDITOR_FIELD(ImGui::DragFloat2(id.c_str(), glm::value_ptr(value)), Vector2, oldValue2);
	}
	bool EditorGUI::Vec2SliderField(PARAMS_SLIDER(Vector2))
	{
		EDITOR_FIELD(ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max), Vector2, oldValue2);
	}
		 
	bool EditorGUI::Vec3Field(PARAMS_CUSTOM_UNDO_REDO(Vector3))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::DragFloat3(id.c_str(), glm::value_ptr(value)), Vector3, oldValue3);
	}
	bool EditorGUI::Vec3SliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(Vector3))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max), Vector3, oldValue3);
	}
	bool EditorGUI::Vec3Field(PARAMS(Vector3))
	{
		EDITOR_FIELD(ImGui::DragFloat3(id.c_str(), glm::value_ptr(value)), Vector3, oldValue3);
	}
	bool EditorGUI::Vec3SliderField(PARAMS_SLIDER(Vector3))
	{
		EDITOR_FIELD(ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max), Vector3, oldValue3);
	}
		 
	bool EditorGUI::Vec4Field(PARAMS_CUSTOM_UNDO_REDO(Vector4))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::DragFloat4(id.c_str(), glm::value_ptr(value)), Vector4, oldValue4);
	}
	bool EditorGUI::Vec4SliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(Vector4))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max), Vector4, oldValue4);
	}
	bool EditorGUI::Vec4Field(PARAMS(Vector4))
	{
		EDITOR_FIELD(ImGui::DragFloat4(id.c_str(), glm::value_ptr(value)), Vector4, oldValue4);
	}
	bool EditorGUI::Vec4SliderField(PARAMS_SLIDER(Vector4))
	{
		EDITOR_FIELD(ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max), Vector4, oldValue4);
	}

	const ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview;
	bool EditorGUI::Color3Field(PARAMS_CUSTOM_UNDO_REDO(Vector3))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value)), Vector3, oldValue3);
	}
	bool EditorGUI::Color4Field(PARAMS_CUSTOM_UNDO_REDO(Vector4))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), colorFlags), Vector4, oldValue4);
	}
	bool EditorGUI::Color4Field(PARAMS_CUSTOM_UNDO_REDO(ImVec4))
	{
		EDITOR_FIELD_CUSTOM_UNDO_REDO(ImGui::ColorEdit4(id.c_str(), &value.x, colorFlags), ImVec4, oldValueim4);
	}
		 
	bool EditorGUI::Color3Field(PARAMS(Vector3))
	{
		EDITOR_FIELD(ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value)), Vector3, oldValue3);
	}
	bool EditorGUI::Color4Field(PARAMS(Vector4))
	{
		EDITOR_FIELD(ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), colorFlags), Vector4, oldValue4);
	}
	bool EditorGUI::Color4Field(PARAMS(ImVec4))
	{
		EDITOR_FIELD(ImGui::ColorEdit4(id.c_str(), &value.x, colorFlags), ImVec4, oldValueim4);
	}

	bool EditorGUI::EditorButton(const char* label, float width, bool isActive)
	{
		bool pressed = false;
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[isActive ? ImGuiCol_TabActive : ImGuiCol_Button]);
		if (ImGui::Button(label, ImVec2(width, -1)))
			pressed = true;
		ImGui::PopStyleColor();
		return pressed;
	}

	bool EditorGUI::EditorImageButton(Ref<Texture2D> icon, ImVec2 size, bool isActive)
	{
		ImVec4 backGroundActive = ImGui::GetStyle().Colors[ImGuiCol_TabActive];
		ImVec4 activeTint = ImGui::GetStyle().Colors[ImGuiCol_Text];
		return ImGui::ImageButton((ImTextureID)(intptr_t)(icon->GetRendererID()), size, ImVec2(0, 1), ImVec2(1, 0), 0, isActive ? backGroundActive : ImVec4(0, 0, 0, 0), activeTint);
	}














	//const std::string s_ImGuiColorKeys[]{
	//	"Text",
	//	"TextDisabled",
	//	"WindowBg",              // Background of normal windows
	//	"ChildBg",               // Background of child windows
	//	"PopupBg",               // Background of popups, menus, tooltips windows
	//	"Border",
	//	"BorderShadow",
	//	"FrameBg",               // Background of checkbox, radio button, plot, slider, text input
	//	"FrameBgHovered",
	//	"FrameBgActive",
	//	"TitleBg",
	//	"TitleBgActive",
	//	"TitleBgCollapsed",
	//	"MenuBarBg",
	//	"ScrollbarBg",
	//	"ScrollbarGrab",
	//	"ScrollbarGrabHovered",
	//	"ScrollbarGrabActive",
	//	"CheckMark",
	//	"SliderGrab",
	//	"SliderGrabActive",
	//	"Button",
	//	"ButtonHovered",
	//	"ButtonActive",
	//	"Header",                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
	//	"HeaderHovered",
	//	"HeaderActive",
	//	"Separator",
	//	"SeparatorHovered",
	//	"SeparatorActive",
	//	"ResizeGrip",
	//	"ResizeGripHovered",
	//	"ResizeGripActive",
	//	"Tab",
	//	"TabHovered",
	//	"TabActive",
	//	"TabUnfocused",
	//	"TabUnfocusedActive",
	//	"DockingPreview",        // Preview overlay color when about to docking something
	//	"DockingEmptyBg",        // Background color for empty node (e.g. CentralNode with no window docked into it)
	//	"PlotLines",
	//	"PlotLinesHovered",
	//	"PlotHistogram",
	//	"PlotHistogramHovered",
	//	"TextSelectedBg",
	//	"DragDropTarget",
	//	"NavHighlight",          // Gamepad/keyboard: current highlighted item
	//	"NavWindowingHighlight", // Highlight window when using CTRL+TAB
	//	"NavWindowingDimBg",     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	//	"ModalWindowDimBg",
	//};

	/*void EditorGUI::DrawEditorColorPickers()
	{*/
		//ImGui::Begin("Editor Colors");
		//ImGui::Columns(2);

		/*ImVec4* colors = ImGui::GetStyle().Colors;

		for (uint32_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			Color4Field(s_ImGuiColorKeys[i], colors[i]);
		}*/

		//ImGui::Columns(1);
		//ImGui::End();
	//}

	//void EditorGUI::InitializeGUIColors()
	//{
	//	//return;
	//	// ImGui Colors
	//	ImVec4* colors = ImGui::GetStyle().Colors;
	//	colors[ImGuiCol_Text]					= ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	//	colors[ImGuiCol_TextDisabled]			= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	//	colors[ImGuiCol_WindowBg]				= ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Window background
	//	colors[ImGuiCol_ChildBg]				= ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	//	colors[ImGuiCol_PopupBg]				= ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	//	colors[ImGuiCol_Border]					= ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
	//	colors[ImGuiCol_BorderShadow]			= ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	//	colors[ImGuiCol_FrameBg]				= ImVec4(0.3f, 0.3f, 0.3f, 0.5f); // Widget backgrounds
	//	colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
	//	colors[ImGuiCol_FrameBgActive]			= ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
	//	colors[ImGuiCol_TitleBg]				= ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
	//	colors[ImGuiCol_TitleBgActive]			= ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
	//	colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
	//	colors[ImGuiCol_MenuBarBg]				= ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
	//	colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	//	colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
	//	colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
	//	colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
	//	colors[ImGuiCol_CheckMark]				= ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
	//	colors[ImGuiCol_SliderGrab]				= ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
	//	colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
	//	colors[ImGuiCol_Button]					= ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
	//	colors[ImGuiCol_ButtonHovered]			= ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
	//	colors[ImGuiCol_ButtonActive]			= ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
	//	colors[ImGuiCol_Header]					= ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
	//	colors[ImGuiCol_HeaderHovered]			= ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
	//	colors[ImGuiCol_HeaderActive]			= ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
	//	colors[ImGuiCol_Separator]				= ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
	//	colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
	//	colors[ImGuiCol_SeparatorActive]		= ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
	//	colors[ImGuiCol_ResizeGrip]				= ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
	//	colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
	//	colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
	//	colors[ImGuiCol_PlotLines]				= ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
	//	colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
	//	colors[ImGuiCol_PlotHistogram]			= ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
	//	colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	//	colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
	//	colors[ImGuiCol_ModalWindowDarkening]	= ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
	//	colors[ImGuiCol_DragDropTarget]			= ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	//	colors[ImGuiCol_NavHighlight]			= ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
	//	colors[ImGuiCol_NavWindowingHighlight]	= ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	//}
}