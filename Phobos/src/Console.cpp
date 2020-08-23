#include "PhobosPCH.h"
#include "Console.h"
#include <Ares.h>
#include <mutex>
#include "spdlog/sinks/base_sink.h"

//#include <imgui.h>
namespace Ares
{

	template<typename Mutex>
	class my_sink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		my_sink(const std::function<void(const std::string&, NotificationType)>& dispatchConsole)
			//: spdlog::sinks::base_sink()

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
			//std::cout << fmt::to_string(formatted);
			
			NotificationType type = NotificationType::Log;
			switch (msg.level)
			{
			case spdlog::level::critical:
				type = NotificationType::Error;
				break;
			case spdlog::level::err:
				type = NotificationType::Error;
				break;
			case spdlog::level::warn:
				type = NotificationType::Warn;
				break;
			case spdlog::level::info:
				type = NotificationType::Info;
				break;
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
				{
					Dispatch(msg, type); 
				}
			}
		);

		s->set_pattern("%^[%T] %n: %v%$");

		//my_sink s;
		//Log::AddSink(s);

		Log::s_CoreLogger->sinks().push_back(s);
		Log::s_ClientLogger->sinks().push_back(s);
	}
	Console::~Console()
	{
		m_Shutdown = true;
	}


	//Notification::Notification(std::string type, std::string message, Ref<Texture2D> icon, int duration)
	Notification::Notification(const std::string& message, NotificationType type)
	{
		//m_icon = icon;
		m_type = type;
		m_message = message;
		//m_notificationId = rand();
	}

	/*void Notification::SetPosition(int x = 1631, int y = 920)
	{
		ImGui::SetNextWindowPos(ImVec2(x, y));
	}*/

	void Notification::Show()
	{
		//bool p1_close = NULL;

		/*ImGui::Begin(
			std::to_string(m_notificationId).c_str(),
			&p1_close,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing
		);*/

		{


			//ImGui::Image((void*)m_icon->GetRendererID(), ImVec2(80, 80));
			//ImGui::SameLine();

			//ImGui::BeginChild("##Information", ImVec2(172, 80));
			{
				//ImGui::Separator();

				if (m_type != NotificationType::Log)
				{

					//ImGui::PushStyleColor(ImGuiCol_Text, s_notificationColors[(uint8_t)m_type]);
					ImGui::PushStyleColor(ImGuiCol_Text, s_notificationColors[(uint8_t)m_type]);
				}

				//auto type = "[" + m_type + "]";
				//auto channelId = "Channel: " + std::to_string(m_notificationId);

				//ImGui::Text(type.c_str());
				//ImGui::SameLine();

				ImGui::Text(m_message.c_str());

				if (m_type != NotificationType::Log)
				{

				ImGui::PopStyleColor();
				}

				//ImGui::Text(channelId.c_str());
				//ImGui::EndChild();
				ImGui::Separator();
			}

			//ImGui::End();

		}

		/*ImGui::SetNextWindowPos(ImVec2(m_base_x_pos, m_base_y_pos - m_base_offset));
		ImGui::Begin("Hazel N2", &p1_close, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);
		{
			ImGui::Image((void*)m_icon->GetRendererID(), ImVec2(80, 80));
			ImGui::SameLine();

			ImGui::BeginChild("##Information", ImVec2(172, 80));
			{
				ImGui::Text("Scanning assets...");
				ImGui::Text("Channel Id: 10093");
				ImGui::Text("Notification Type: Info");
				ImGui::EndChild();
			}

			ImGui::End();
		}*/
	}






	void Console::Dispatch(
		const std::string& message, NotificationType type
		//Notification n
	)
	{
		/*notifications.push_back(n);*/

		notifications.push_back({ message, type });
		//m_count = notifications.size();
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

			/*if (notifications.size() > 0)
			{*/
				//Expire();
				for (int i = 0; i < notifications.size(); i++)
				{
					/*if (i == 0)
					{
						notifications[i].SetPosition(1631, 920);*/
						notifications[i].Show();
					/*}
					else {
						int newOffset = m_offset * i;
						notifications[i].SetPosition(1631, 920 - newOffset);
						notifications[i].Show();
					}*/
				}
			//}
		}
		ImGui::End();
	}

	/*void NotificationManager::Expire()
	{

	}*/
}
