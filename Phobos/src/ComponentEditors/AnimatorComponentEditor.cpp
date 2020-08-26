#include "PhobosPCH.h"
#include <Ares.h>
#include "../EditorGUI.h"
#include "AnimatorComponentEditor.h"

namespace Ares
{

	void AnimatorComponent::OnDrawImGui()
	{

		// TODO: render animation names / assets


		if (ImGui::Button(Animator.m_AnimationPlaying ? "Pause" : "Play"))
			Animator.m_AnimationPlaying = !Animator.m_AnimationPlaying;

		ImGui::SliderFloat("##AnimationTime", &Animator.m_AnimationTime, 0.0f, (Animator.m_Animations[Animator.m_CurrentAnimIndex]->m_Duration));
		ImGui::DragFloat("Time Scale", &Animator.m_Speed, 0.05f, 0.0f, 10.0f);
	}
}