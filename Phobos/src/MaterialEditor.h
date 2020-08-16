#pragma once
#include <Ares.h>

namespace Ares
{
	class MaterialEditor
	{
	public:

		// TODO: ranges for sliders, and color specification for vec4's, toggle for floats
		static void DrawMaterial(Ref<Material> material);
	private:

		static void AddTextureToMaterial(Ref<Material> material, Ref<Texture> tex, const uint32_t& slot);
	};
}