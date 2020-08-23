#include "PhobosPCH.h"
#include <Ares.h>
#include "ComponentEditor.h"
//#include "MaterialEditor.h"
namespace Ares
{

	void Component::OnDrawImGui()
	{
		ImGui::Text("================");
		ImGui::TextWrapped("No 'OnDrawImGui' implemented for this component!");
		ImGui::Text("================");
	}
}