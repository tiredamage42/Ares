
#include <Ares.h>
#include <Ares/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Ares
{

	class Phobos : public Application 
	{
	public:
		Phobos() 
			: Application("Phobos (Ares Editor)")
		{
			PushLayer(new EditorLayer());
		}
		~Phobos() 
		{

		}
	};

	Application* CreateApplication() 
	{
		return new Phobos();
	}
}
