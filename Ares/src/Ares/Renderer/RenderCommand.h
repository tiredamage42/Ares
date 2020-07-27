#pragma once

#include "Ares/Renderer/RendererAPI.h"
namespace Ares {
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}
		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}
		inline static void Clear(float r, float g, float b, float a)
		{
			s_RendererAPI->Clear(r, g, b, a);
		}
		inline static void DrawIndexed(uint32_t indexCount, bool depthTest)
		{
			s_RendererAPI->DrawIndexed(indexCount, depthTest);
		}
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};
}