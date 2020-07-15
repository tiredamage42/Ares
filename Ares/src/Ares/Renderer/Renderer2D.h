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

		static void Flush();

		// Primitives
		/*static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);*/


		static void DrawQuad(
			const glm::vec3& position, 
			float rotation = 0.0f, 
			const glm::vec2& size = glm::vec2(1.0f),
			const Ref<Texture2D>& texture = nullptr, 
			float tiling = 1.0f, 
			const glm::vec4& color = glm::vec4(1.0f)
		);
		static void DrawQuad(
			const glm::vec2& position,
			float rotation = 0.0f,
			const glm::vec2& size = glm::vec2(1.0f),
			const Ref<Texture2D>& texture = nullptr,
			float tiling = 1.0f,
			const glm::vec4& color = glm::vec4(1.0f)
		);



		struct Statistics
		{

			std::array<float, 100> FrameRenderTime; // collect render time for multiple frames
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t TextureCount = 0;
			uint32_t FrameCount = 0;
			float CurrentFrameBeginTime = 0.0f;
			float TotalFrameRenderTime = 0.0f;

			/*uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;*/

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();
		static void ResetStats();
		static void StatsBeginFrame();
		static void StatsEndFrame();


	private:

		static void FlushAndReset();

	};

}