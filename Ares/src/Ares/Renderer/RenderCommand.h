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

		/*inline static unsigned int Clear(void* datablock)
		{
			float* data = (float*)datablock;

			float r = *data++;
			float g = *data++;
			float b = *data++;
			float a = *data;

			s_RendererAPI->Clear(r, g, b, a);

			return sizeof(float) * 4;
		}*/


		/*inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}*/
		inline static void DrawIndexed(uint32_t indexCount)
		{
			s_RendererAPI->DrawIndexed(indexCount);
		}

	private:

		static Scope<RendererAPI> s_RendererAPI;

	};
}