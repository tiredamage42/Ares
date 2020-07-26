#pragma once

#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Texture.h"

namespace Ares {
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(
			const glm::vec3& position, float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f),
			const Ref<Texture2D>& texture = nullptr,
			const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec2& offset = glm::vec2(0.0f),
			const glm::vec4& color = glm::vec4(1.0f)
		);
		static void DrawQuad(
			const glm::vec2& position, float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f),
			const Ref<Texture2D>& texture = nullptr,
			const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec2& offset = glm::vec2(0.0f),
			const glm::vec4& color = glm::vec4(1.0f)
		);
		static void DrawQuad(
			const glm::mat4& transform,
			const Ref<Texture2D>& texture = nullptr,
			const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec2& offset = glm::vec2(0.0f),
			const glm::vec4& color = glm::vec4(1.0f)
		);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();

		inline static void SetMaxQuadsPerDraw(uint32_t maxQuads) { s_MaxQuadsPerDraw = maxQuads; }

	private:

		inline static uint32_t s_MaxQuadsPerDraw = 10000;

		static void FlushAndReset();
	};
}