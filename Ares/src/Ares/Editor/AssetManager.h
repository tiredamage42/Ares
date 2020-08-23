#pragma once

#include <filesystem>
//#include "AssetTypes.h"
#include "DirectoryInformation.h"

namespace Ares
{
	class AssetManager
	{
	public:
		//static std::string ParseFilename(std::string const& str, const char delim, std::vector<std::string>& out);
		static std::string ParseFilename(std::string const& str, const char delim);

		static std::string ParseFiletype(std::string filename);

		/*static void ProcessAseets(std::string assetType);
		static void InitiateAssetConversion(std::string assetPath, std::string conversionType);*/

		static std::vector<DirectoryInformation> GetFsContents();
		static std::vector<DirectoryInformation> ReadDirectory(std::string path);
		//static std::vector<DirectoryInformation> ReadDirectoryRecursive(std::string path);
		static std::string GetParentPath(std::string path);

		static std::vector<std::string> GetDirectories(std::string path);
		static std::vector<std::string> SearchFiles(std::string query);
		static bool MoveFileTo(std::string filePath, std::string movePath);

		static std::string StripExtras(std::string filename);


		static bool ImportAsset(const std::string& filePath, const std::string& targetDir);

	//private:
		//void ImportAsset(std::string assetPath, std::string assetName);
	};
}
