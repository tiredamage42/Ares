#pragma once

//#include "Ares/Editor/Notification.h"
#include <vector>
#include <string>
//#include <imgui.h>
namespace Ares
{

	enum class NotificationType : uint8_t
	{
		Log = 3, Info = 0, Warn = 1, Error = 2
	};


	class Notification
	{
	public:
		//Notification(std::string type, std::string message, Ref<Texture2D> icon);
		Notification(const std::string& message, NotificationType type);

		//Notification(std::string type, std::string message, Ref<Texture2D> icon, int duration);
		//void SetPosition(int x, int y);
		void Show();
	private:
		//Ref<Texture2D> m_icon;
		//int m_notificationId;
		//std::string m_type;

		NotificationType m_type;
		std::string m_message;
	};

	class Console
	{
	public:
		Console();
		~Console();
		void Dispatch(const std::string& message, NotificationType type);
		//void Dispatch(Notification n);
		void Render();
		//void Expire();
	private:
		bool m_Shutdown = false;
		std::vector<Notification> notifications;
		//int m_count = 0;
		//int m_offset = 100;
	};


}
