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
	};
}