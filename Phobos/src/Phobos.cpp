
#include <Ares.h>
#include <Ares/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Ares
{

	class Phobos : public Application 
	{
	public:
		Phobos(const WindowProps& props)
			: Application(props)
		{
			PushLayer(new EditorLayer());
		}
		~Phobos() 
		{

		}
	};

	Application* CreateApplication() 
	{
		return new Phobos({ "Phobos (Ares Editor)", 1280, 720 });
	}
}
