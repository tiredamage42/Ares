#pragma once
#include <Ares.h>

namespace Ares
{
	class InspectorPanel
	{
	public:
		static void DrawInspectorForEntity(const Entity& entity);

	private:

		template<typename T>//, typename UIFunction>
		static void DrawComponent(const std::string& name, Entity entity);// , UIFunction uiFunction);

		static void DrawComponents(const Entity& entity);
	};
}