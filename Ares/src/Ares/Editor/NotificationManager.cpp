#include "AresPCH.h"
#include "NotificationManager.h"
#include <imgui.h>
namespace Ares
{



	static const ImVec4 s_notificationColors[] = {
		ImVec4(0.9f, 0.75f, 0.2f, 1.0f),
		ImVec4(0.9f, 0.2f, 0.2f, 1.0f), 
	};





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






	void NotificationManager::Dispatch(
		const std::string& message, NotificationType type
		//Notification n
	)
	{
		/*notifications.push_back(n);*/

		notifications.push_back({ message, type });
		//m_count = notifications.size();
	}

	void NotificationManager::Render()
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
