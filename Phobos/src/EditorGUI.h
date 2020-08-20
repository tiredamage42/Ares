#pragma once
#include <Ares.h>
#include <string>
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

		static void InitializeGUIColors();
		static void DrawEditorColorPickers();

	};
}