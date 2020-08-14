#pragma once
#include <string>

namespace Ares
{
	class FileUtils
	{
	public:

		static std::string GetFileContents(const std::string& filePath, bool& success);
		static std::string ExtractFileNameFromPath(const std::string& filePath);
		static std::string RemoveDirectoryFromPath(const std::string& filePath);
	};
}