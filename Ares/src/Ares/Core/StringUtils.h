#pragma once

#include <string>
namespace Ares
{
	class StringUtils
	{
	public:
		static inline std::hash<std::string> hasher;
		static size_t String2Hash(const std::string& str)
		{
			return hasher(str);
		}

		static std::string ExtractStringFromBetweenBrackets(std::string& str, size_t startIDX, bool deleteFromOriginal = false, const std::string& brackets = "{}")
		{
			size_t extractStartIDX = str.find_first_of(brackets[0], startIDX) + 1;
			size_t extractEndIDX = str.find_first_of(brackets[1], extractStartIDX);
			std::string r = str.substr(extractStartIDX, extractEndIDX - extractStartIDX);

			if (deleteFromOriginal)
			{
				str.erase(startIDX, (extractEndIDX + 1) - startIDX);
			}

			return r;
		}

		static void RemoveWhiteSpaceFrom(std::string& str)
		{
			str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
		}

		static std::string ReplaceInString(std::string str, const std::string& search, const std::string& replace) 
		{
			size_t pos = 0;
			while ((pos = str.find(search, pos)) != std::string::npos) {
				str.replace(pos, search.length(), replace);
				pos += replace.length();
			}
			return str;
		}
	};
}