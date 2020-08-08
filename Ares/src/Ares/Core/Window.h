#pragma once
#include "AresPCH.h"
#include "Ares/Core/Core.h"
#include "Ares/Events/Event.h"

namespace Ares {
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(
			const std::string& title = "Ares Engine",
			uint32_t width = 1280,
			uint32_t height = 720
		) : Title(title), Width(width), Height(height)
		{
		}
	};

	// interface representing a desktop system based window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() = default;
		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::pair<uint32_t, uint32_t> GetSize() const = 0;

		virtual std::pair<float, float> GetWindowPos() const = 0;


		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}