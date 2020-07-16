#pragma once

#include "Ares/Core/Layer.h"
//#include "Ares/Events/ApplicationEvent.h"
//#include "Ares/Events/KeyEvent.h"
//#include "Ares/Events/MouseEvent.h"

namespace Ares {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;
		void BeginImGui();
		void EndImGui();
	};
}