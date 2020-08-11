#pragma once
#include <string>

namespace Ares
{

	struct DirectoryInformation
	{
		std::string filename;
		std::string fileType;
		std::string absolutePath;
		bool isFile;

	public:
		DirectoryInformation(std::string fname, std::string ftype, std::string absPath, bool isF) {
			filename = fname;
			fileType = ftype;
			absolutePath = absPath;
			isFile = isF;
		}
	};
}
