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

		static std::string RemoveCommentsFrom(const std::string& str)
		{
			size_t n = str.length();
			std::string res;

			// Flags to indicate that single line and multpile line comments 
			// have started or not. 
			bool s_cmt = false;
			bool m_cmt = false;

			// Traverse the given program 
			for (size_t i = 0; i < n; i++)
			{
				const char& c = str[i];

				// If single line comment flag is on, then check for end of it 
				if (s_cmt && (c == '\n' || c == '\r'))
					s_cmt = false;

				// If multiple line comment is on, then check for end of it 
				else if (m_cmt && c == '*' && str[i + 1] == '/')
					m_cmt = false, i++;

				// If this character is in a comment, ignore it 
				else if (s_cmt || m_cmt)
					continue;

				// Check for beginning of comments and set the approproate flags 
				else if (c == '/' && str[i + 1] == '/')
					s_cmt = true, i++;
				else if (c == '/' && str[i + 1] == '*')
					m_cmt = true, i++;

				// If current character is a non-comment character, append it to res 
				else  res += c;
			}
			return res;
		}
	};
}