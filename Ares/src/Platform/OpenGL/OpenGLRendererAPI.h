#pragma once

#include "Ares/Renderer/RendererAPI.h"

namespace Ares {
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
		virtual void Clear(float r, float g, float b, float a) override;
		virtual void DrawIndexed(uint32_t indexCount, bool depthTest) override;
	};
}