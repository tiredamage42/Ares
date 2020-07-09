#pragma once

#include "Ares/Layer.h"
//#include "Ares/Events/ApplicationEvent.h"
//#include "Ares/Events/KeyEvent.h"
//#include "Ares/Events/MouseEvent.h"

namespace Ares {
	class ARES_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiDraw() override;

		
		void BeginImGui();
		void EndImGui();
	private:
		float m_Time = 0.0f;
	};
}