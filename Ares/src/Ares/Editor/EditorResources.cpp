#include "AresPCH.h"
#include "EditorResources.h"
namespace Ares
{
	std::unordered_map<std::string, Ref<Texture2D>> EditorResources::s_TexturesMap;

	Ref<Texture2D> EditorResources::GetTexture(const std::string& filePath)
	{

		if (s_TexturesMap.find(filePath) != s_TexturesMap.end())
			return s_TexturesMap.at(filePath);

		//std::string str = "EditorResources/" + filePath;
		Ref<Texture2D> texture = Texture2D::Create("EditorResources/" + filePath, FilterType::Point, false);
		s_TexturesMap[filePath] = texture;

		return texture;
	}
}