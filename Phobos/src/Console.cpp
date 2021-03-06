#include "PhobosPCH.h"
#include "Console.h"
#include <Ares.h>
#include <mutex>
#include "spdlog/sinks/base_sink.h"
namespace Ares
{

	template<typename Mutex>
	class my_sink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		my_sink(const std::function<void(const std::string&, NotificationType)>& dispatchConsole)
		{
			m_DispatchConsole = dispatchConsole;
		}
	protected:
		std::function<void(const std::string&, NotificationType)> m_DispatchConsole;

		void sink_it_(const spdlog::details::log_msg& msg) override
		{

			// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
			// msg.raw contains pre formatted log

			// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
			spdlog::memory_buf_t formatted;
			base_sink<Mutex>::formatter_->format(msg, formatted);
			
			NotificationType type = NotificationType::Log;
			switch (msg.level)
			{
			case spdlog::level::critical:	type = NotificationType::Error; break;
			case spdlog::level::err:		type = NotificationType::Error; break;
			case spdlog::level::warn:		type = NotificationType::Warn; break;
			case spdlog::level::info:		type = NotificationType::Info; break;
			}
			m_DispatchConsole(fmt::to_string(formatted), type);
		}

		void flush_() override
		{
			std::cout << std::flush;
		}
	};

	using my_sink_mt = my_sink<std::mutex>;

	static const ImVec4 s_notificationColors[] = {
		ImVec4(0.2f, 0.9f, 0.2f, 1.0f),
		ImVec4(0.9f, 0.75f, 0.2f, 1.0f),
		ImVec4(0.9f, 0.2f, 0.2f, 1.0f), 
	};

	Console::Console()
	{
		auto s = std::make_shared<my_sink_mt>(
			[=](const std::string& msg, NotificationType type) { 
				if (!m_Shutdown)
					Dispatch(msg, type); 
			}
		);

		s->set_pattern("%^[%T] %n: %v%$");

		Log::s_CoreLogger->sinks().push_back(s);
		Log::s_ClientLogger->sinks().push_back(s);
	}
	Console::~Console()
	{
		m_Shutdown = true;
	}

	void Console::Dispatch(const std::string& message, NotificationType type)
	{
		notifications.push_back({ message, type });
	}

	void Console::Render()
	{
		ImGui::Begin("Notifications");
		{
			if (ImGui::Button("Clear"))
			{
				notifications.clear();
			}
			ImGui::Separator();

			for (int i = 0; i < notifications.size(); i++)
			{

				if (notifications[i].Type != NotificationType::Log)
					ImGui::PushStyleColor(ImGuiCol_Text, s_notificationColors[(uint8_t)notifications[i].Type]);

				ImGui::Text(notifications[i].Message.c_str());

				if (notifications[i].Type != NotificationType::Log)
					ImGui::PopStyleColor();

				ImGui::Separator();
			}
		}
		ImGui::End();
	}
}
