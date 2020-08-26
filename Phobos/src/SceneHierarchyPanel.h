#pragma once
#include <Ares.h>
namespace Ares {

	class SceneHierarchyPanel
	{
	public:
		static void Draw(Ref<Scene> scene, Entity& selectedEntity, Entity& doubleClickedEntity, bool& windowFocused);
	private:
		static void DrawEntityNode(const Entity& entity, Entity& selectedEntity, Entity& doubleClickedEntity, Ref<Scene> scene);
	};

}