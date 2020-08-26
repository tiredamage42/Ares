#pragma once
#include <vector>
#include <string>
namespace Ares
{
	enum class NotificationType : uint8_t
	{
		Log = 3, Info = 0, Warn = 1, Error = 2
	};

	class Console
	{
	public:
		Console();
		~Console();
		void Dispatch(const std::string& message, NotificationType type);
		void Render();
	private:

		struct Notification
		{
			std::string Message;
			NotificationType Type;
		};

		bool m_Shutdown = false;
		std::vector<Notification> notifications;
	};
}
