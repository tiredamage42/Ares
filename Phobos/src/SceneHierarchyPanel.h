#pragma once
#include <Ares.h>
namespace Ares {

	class SceneHierarchyPanel
	{
	public:
		static void Draw(Ref<Scene> scene, Entity& selectedEntity, bool& selectionChanged, Entity& deletedEntity);
	private:
		static void DrawEntityNode(const Entity& entity, const Entity& selectedEntity, bool& clicked, bool& deleted);
	};

}