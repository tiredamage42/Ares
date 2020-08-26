#include "PhobosPCH.h"
#include <Ares.h>
#include "../EditorGUI.h"
#include "CameraComponentEditor.h"

namespace Ares
{
	void CameraComponent::OnDrawImGui()
	{
		// Projection Type

		EditorGUI::BeginPropertyGrid();

		SceneCamera::ProjectionType projection = Camera.GetProjectionType();

		std::function<void(SceneCamera::ProjectionType)> undo = [=](SceneCamera::ProjectionType v) { this->Camera.SetProjectionType(v); };
		if (EditorGUI::EnumField("Projection", projection, undo, SceneCamera::AllProjectionTypes, SceneCamera::AllProjectionTypeNames))
			this->Camera.SetProjectionType(projection);

		// Perspective parameters
		if (Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
		{
			float verticalFOV = Camera.GetPerspectiveVerticalFOV();
			if (EditorGUI::FloatField("Vertical FOV", verticalFOV, [=](float v) { this->Camera.SetPerspectiveVerticalFOV(v); }))
				Camera.SetPerspectiveVerticalFOV(verticalFOV);

			float nearClip = Camera.GetPerspectiveNearClip();
			if (EditorGUI::FloatField("Near Clip", nearClip, [=](float v) { this->Camera.SetPerspectiveNearClip(v); }))
				Camera.SetPerspectiveNearClip(nearClip);

			float farClip = Camera.GetPerspectiveFarClip();
			if (EditorGUI::FloatField("Far Clip", farClip, [=](float v) { this->Camera.SetPerspectiveFarClip(v); }))
				Camera.SetPerspectiveFarClip(farClip);
		}

		// Orthographic parameters
		else if (Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
		{

			float orthoSize = Camera.GetOrthographicSize();
			if (EditorGUI::FloatField("Size", orthoSize, [=](float v) { this->Camera.SetOrthographicSize(v); }))
				Camera.SetOrthographicSize(orthoSize);

			float nearClip = Camera.GetOrthographicNearClip();
			if (EditorGUI::FloatField("Near Clip", nearClip, [=](float v) { this->Camera.SetOrthographicNearClip(v); }))
				Camera.SetOrthographicNearClip(nearClip);

			float farClip = Camera.GetOrthographicFarClip();
			if (EditorGUI::FloatField("Far Clip", farClip, [=](float v) { this->Camera.SetOrthographicFarClip(v); }))
				Camera.SetOrthographicFarClip(farClip);

		}

		EditorGUI::EndPropertyGrid();
	}
}