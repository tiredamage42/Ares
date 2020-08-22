#pragma once

//#include <glm/glm.hpp>
#include "Ares/Math/Math.h"
#include "Ares/Renderer/VertexArray.h"

namespace Ares {

	// TODO: move into separate header
	enum class PrimitiveType
	{
		None = 0, Triangles, Lines
	};

	struct RenderAPICapabilities
	{
		std::string Vendor;
		std::string Renderer;
		std::string Version;

		int MaxSamples = 0;
		float MaxAnisotropy = 0.0f;
		int MaxTextureUnits = 0;
	};

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Clear() = 0;
		virtual void Clear(float r, float g, float b, float a, bool clearColor, bool clearDepth, bool clearStencil) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void DrawIndexed(uint32_t indexCount, PrimitiveType type, bool depthTest) = 0;
		virtual void SetLineThickness(float thickness) = 0;

		static RenderAPICapabilities& GetCapabilities()
		{
			static RenderAPICapabilities capabilities;
			return capabilities;
		}


		inline static API GetAPI() { return s_API; }

		static Scope<RendererAPI> Create();

	private:

		static API s_API;

	};
}