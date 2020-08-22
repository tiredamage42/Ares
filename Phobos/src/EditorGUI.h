#pragma once
#include <Ares.h>
#include <string>
#include "EditorUtility.h"

namespace Ares
{

#define CUSTOM_UNDO_REDO(TYPE)					const std::function<void(TYPE)>& CUSTOM_UNDO

#define PARAMS(TYPE)							const std::string& name, TYPE& value
#define PARAMS_CUSTOM_UNDO_REDO(TYPE)			PARAMS(TYPE), CUSTOM_UNDO_REDO(TYPE)
#define PARAMS_SLIDER(TYPE)						PARAMS(TYPE), float min, float max
#define PARAMS_SLIDER_CUSTOM_UNDO_REDO(TYPE)	PARAMS_SLIDER(TYPE), CUSTOM_UNDO_REDO(TYPE)

	
	// ImGui UI helpers
	class EditorGUI
	{
	public:
		static void ShowTooltip(const char* text);

		//static bool StringField(PARAMS_CUSTOM_UNDO_REDO(std::string));
		//static bool StringField(PARAMS(std::string));

		static bool ToggleField(PARAMS_CUSTOM_UNDO_REDO(bool));
		static bool ToggleField(PARAMS(bool));

		static bool FloatField(PARAMS_CUSTOM_UNDO_REDO(float));
		static bool FloatSliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(float));
		static bool FloatField(PARAMS(float));
		static bool FloatSliderField(PARAMS_SLIDER(float));

		static bool IntField(PARAMS_CUSTOM_UNDO_REDO(int));
		static bool IntSliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(int));
		static bool IntField(PARAMS(int));
		static bool IntSliderField(PARAMS_SLIDER(int));

		static bool Vec2Field(PARAMS_CUSTOM_UNDO_REDO(Vector2));
		static bool Vec2SliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(Vector2));
		static bool Vec2Field(PARAMS(Vector2));
		static bool Vec2SliderField(PARAMS_SLIDER(Vector2));

		static bool Vec3Field(PARAMS_CUSTOM_UNDO_REDO(Vector3));
		static bool Vec3SliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(Vector3));
		static bool Vec3Field(PARAMS(Vector3));
		static bool Vec3SliderField(PARAMS_SLIDER(Vector3));

		static bool Vec4Field(PARAMS_CUSTOM_UNDO_REDO(Vector4));
		static bool Vec4SliderField(PARAMS_SLIDER_CUSTOM_UNDO_REDO(Vector4));
		static bool Vec4Field(PARAMS(Vector4));
		static bool Vec4SliderField(PARAMS_SLIDER(Vector4));

		static bool Color3Field(PARAMS_CUSTOM_UNDO_REDO(Vector3));
		static bool Color4Field(PARAMS_CUSTOM_UNDO_REDO(Vector4));
		static bool Color4Field(PARAMS_CUSTOM_UNDO_REDO(ImVec4));

		static bool Color3Field(PARAMS(Vector3));
		static bool Color4Field(PARAMS(Vector4));
		static bool Color4Field(PARAMS(ImVec4));








		/*
		template<typename T>
		static bool FlagsField(PARAMS(uint32_t), const T allFlags[], const char* allFlagNames[]);
		template<typename T>
		static bool FlagsField(PARAMS_CUSTOM_UNDO_REDO(uint32_t), const T allFlags[], const char* allFlagNames[]);
		
		template<typename T>
		static bool EnumField(PARAMS(T), const T allEnums[], const char* allEnumNames[]);
		template<typename T>
		static bool EnumField(PARAMS_CUSTOM_UNDO_REDO(T), const T allEnums[], const char* allEnumNames[]);
		*/






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
	if (ImGui::BeginCombo(("##" + name).c_str(), preview.c_str())) \
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
		static inline bool FlagsField(PARAMS(uint32_t), const T allFlags[], const char* allFlagNames[])
		{
			FLAGS_FIELD(
				EditorUtility::AddToUndoStack({
					[&value, ov]() { value = ov; },
					[&value, nv]() { value = nv; }
					});
			)
		}
		template<typename T>
		static inline bool FlagsField(PARAMS_CUSTOM_UNDO_REDO(uint32_t), const T allFlags[], const char* allFlagNames[])
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
	if (ImGui::BeginCombo(("##" + name).c_str(), allEnumNames[preview])) \
	{ \
		for (uint32_t i = 0; i < count; i++) \
		{ \
			bool isSelected = preview == i; \
			if (ImGui::Selectable(allEnumNames[i], &isSelected, ImGuiItemFlags_SelectableDontClosePopup)) \
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
		static inline bool EnumField(PARAMS_CUSTOM_UNDO_REDO(T), const T allEnums[], const char* allEnumNames[])
		{
			ENUM_FIELD(
				EditorUtility::AddToUndoStack({
					[&value, ov]() { value = ov; },
					[&value, nv]() { value = nv; }
					});
			)
		}
		template<typename T>
		static inline bool EnumField(PARAMS(T), const T allEnums[], const char* allEnumNames[])
		{
			ENUM_FIELD(
				EditorUtility::AddToUndoStack({
					[CUSTOM_UNDO, ov]() { CUSTOM_UNDO(ov); },
					[CUSTOM_UNDO, nv]() { CUSTOM_UNDO(nv); }
					});
			)
		}




















		static bool EditorButton(const char* label, float width, bool isActive);
		static bool EditorImageButton(Ref<Texture2D> icon, ImVec2 size, bool isActive);

		//static void InitializeGUIColors();
		//static void DrawEditorColorPickers();

	};
}