#include "AresPCH.h"
#include "FileUtils.h"
namespace Ares
{
	std::string FileUtils::GetFileContents(const std::string& filePath, bool& success)
	{
		success = false;
		std::string result;
		std::ifstream in(filePath, std::ios::in | std::ios::binary);
		if (in)
		{
			// go to the end
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				// resize reult to size of file
				result.resize(size);
				// go to the beginning of the files
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				success = true;
			}
			else
			{
				ARES_CORE_ERROR("Could not read from file '{0}'", filePath);
			}
		}
		else
		{
			ARES_CORE_ERROR("Could not open file: {0}", filePath);
		}
		in.close();
		return result;
	}
	std::string FileUtils::ExtractFileNameFromPath(const std::string& filePath)
	{
		// extract name from filepath `assets/shaders/shader.glsl`
		// find last of forward slash or back slash
		size_t lastSlash = filePath.find_last_of("/\\");
		size_t nameStart = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		size_t extensionStart = filePath.rfind('.');
		size_t nameEnd = extensionStart == std::string::npos ? filePath.size() : extensionStart;
		return filePath.substr(nameStart, nameEnd - nameStart);
	}
	std::string FileUtils::RemoveDirectoryFromPath(const std::string& filePath)
	{
		size_t lastSlash = filePath.find_last_of("/\\");
		size_t nameStart = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		return filePath.substr(nameStart, filePath.size() - nameStart);
	}
}