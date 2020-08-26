#pragma once

#include "Material.h"

namespace Ares
{
	class Lighting
	{
		friend class Renderer;
	public:
		static void UpdateGI(Ref<Material> skyboxMaterial);

		static const Ref<TextureCube> GetReflectionsCubeMap();
		
	private:
		static void Init();
		static void Shutdown();
		static void CaptureCurrentSkybox(Ref<Material> skyboxMaterial);


	};
}