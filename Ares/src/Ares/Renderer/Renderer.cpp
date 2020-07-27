#include "AresPCH.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Renderer/Renderer2D.h"
namespace Ares {

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	
	RenderCommandQueue Renderer::s_CommandQueue;

	void Renderer::Init()
	{
		Submit([]() { 
			RenderCommand::Init(); 
		});
		Renderer2D::Init();
	}
	
	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::DrawIndexed(uint32_t count, bool depthTest)
	{
		Submit([=]() {
			RenderCommand::DrawIndexed(count, depthTest);
		});
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Submit([=]() { 
			RenderCommand::SetViewport(0, 0, width, height);
		});
	}

	void Renderer::Clear(float r, float g, float b, float a)
	{
		Submit([=]() {
			RenderCommand::Clear(r, g, b, a);
		});
	}	
	void Renderer::WaitAndRender()
	{
		s_CommandQueue.Execute();
	}
}