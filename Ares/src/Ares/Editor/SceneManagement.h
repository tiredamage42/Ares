#pragma once
#include <string>
namespace Ares
{
	class SceneManagement
	{
		static void LoadScene(std::string scenePath);
		static void SaveScene(std::string name, std::string scenePath);
	};
}
