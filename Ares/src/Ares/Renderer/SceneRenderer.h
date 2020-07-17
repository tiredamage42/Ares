#pragma once

#include "Ares/Scene/Entity.h"

namespace Ares
{
	class SceneRenderer
	{
	public:
		static void SubmitEntity(Entity* entity);
		static void EndScene();
		static void CreateEnvironmentMap(const std::string& filepath);
		

	private:

	};

}