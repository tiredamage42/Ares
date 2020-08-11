#pragma once
#include "Ares/Renderer/Texture.h"
#include <string>
#include <unordered_map>

namespace Ares
{
	class EditorResources
	{
	public:
		static Ref<Texture2D> GetTexture(const std::string& filepath);
	private:
		static std::unordered_map<std::string, Ref<Texture2D>> s_TexturesMap;
	};
}