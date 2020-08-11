#include "AresPCH.h"
#include "FileSystemWatcher.h"

namespace Ares
{
	DWORD WINAPI FileSystemWatcher::StartWatcherThread(LPVOID lparam)
	{
		LPCWSTR		m_fp = L"assets";
		char* m_buf[1024] = { 0 };
		bool		m_watch;
		OVERLAPPED	m_ov;
		HANDLE		m_handle;
		DWORD		m_dwBytesReturned;

		m_ov = { 0 };
		m_dwBytesReturned = 0;
		m_handle = NULL;

		m_handle = CreateFile(
			m_fp,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL
		);

		ZeroMemory(&m_ov, sizeof(m_ov));

		if (m_handle == INVALID_HANDLE_VALUE)
			ARES_CORE_ERROR("Unable to accquire directory handle [ERRCODE] " + GetLastError());

		m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_watch = true;

		while (m_watch)
		{
			DWORD status = ReadDirectoryChangesW(
				m_handle,
				m_buf,
				sizeof(m_buf),
				TRUE,
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME,
				&m_dwBytesReturned,
				&m_ov,
				NULL
			);

			if (!status)
				ARES_CORE_ERROR(GetLastError());

			DWORD waitOperation = WaitForSingleObject(m_ov.hEvent, 5000);

			if (waitOperation == WAIT_OBJECT_0)
			{
				BOOL overlap_result = GetOverlappedResult(m_handle, &m_ov, &m_dwBytesReturned, TRUE);
				if (overlap_result)
				{
					FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&m_buf);

					if (fni->Action == 1)
					{
						/*Create Opertaion*/
						ARES_CORE_ERROR("Created New File With Name: " + wchar_to_string(fni->FileName));
					}

					if (fni->Action == 2)
					{
						/*Delete Operation*/
						ARES_CORE_ERROR("Deleted File : " + wchar_to_string(fni->FileName));
					}

					if (fni->Action == 4)
					{
						/*Rename Operation*/
						ARES_CORE_ERROR("Renamed File With New Name: " + wchar_to_string(fni->FileName));
					}
				}
			}
		}

		return 0;
	}

	void FileSystemWatcher::Watch()
	{
		DWORD threadId;
		auto thread = CreateThread(NULL, 0, StartWatcherThread, 0, 0, &threadId);
	}

	std::string FileSystemWatcher::wchar_to_string(wchar_t* input)
	{
		std::wstring string_input(input);
		std::string converted(string_input.begin(), string_input.end());
		return converted;
	}
}
