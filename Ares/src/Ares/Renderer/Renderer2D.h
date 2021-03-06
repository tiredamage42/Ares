#pragma once

#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Texture.h"

namespace Ares {
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		//static void BeginScene(const OrthographicCamera& camera);
		//static void BeginScene(const glm::mat4& projection, const glm::mat4& transform);
		static void BeginScene(const glm::mat4& viewProj, const Vector3& cameraPosition);// , bool depthTest = true);

		
		static void EndScene(bool quadDT = true, bool quadNDT = true, bool linesDT = true, bool linesNDT = true);

		static void SubmitQuad(
			const glm::vec3& position, float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f),
			const Ref<Texture2D>& texture = nullptr,
			const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec2& offset = glm::vec2(0.0f),
			const glm::vec4& color = glm::vec4(1.0f), bool depthTest = true
		);
		static void SubmitQuad(
			const glm::vec2& position, float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f),
			const Ref<Texture2D>& texture = nullptr,
			const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec2& offset = glm::vec2(0.0f),
			const glm::vec4& color = glm::vec4(1.0f), bool depthTest = true
		);
		static void SubmitQuad(
			const glm::mat4& transform,
			const Ref<Texture2D>& texture = nullptr,
			const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec2& offset = glm::vec2(0.0f),
			const glm::vec4& color = glm::vec4(1.0f), bool depthTest = true
		);

		static void SubmitLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0f), bool depthTest = true, const Vector2& cameraRange = { 100, 200 });


		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t LineCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static Statistics GetStats();

		inline static void SetMaxQuadsPerDraw(uint32_t maxQuads) { s_MaxQuadsPerDraw = maxQuads; }

	private:

		inline static uint32_t s_MaxQuadsPerDraw = 10000;

		//static void FlushAndReset(bool quadDT, bool quadNDT, bool linesDT, bool linesNDT);
		//static void FlushAndResetLines();

		static void ResetQuadsDepthTest();
		static void ResetQuadsNonDepthTest();
		static void ResetLinesDepthTest();
		static void ResetLinesNonDepthTest();

		static void EndSceneLinesNonDepthTest();
		static void EndSceneLinesDepthTest();
		static void EndSceneQuadsNonDepthTest();
		static void EndSceneQuadsDepthTest();

	};
}