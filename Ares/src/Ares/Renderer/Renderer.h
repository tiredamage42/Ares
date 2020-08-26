#pragma once

#include "Ares/Renderer/RenderCommand.h"
#include "Ares/Renderer/Camera.h"
#include "Ares/Renderer/Shader.h"
#include "Ares/Renderer/RenderCommandQueue.h"

#include "Ares/Renderer/RenderPass.h"
#include "Ares/Renderer/Mesh.h"
#include "Ares/Renderer/Material.h"

namespace Ares {

	class Renderer
	{
	public:
		typedef void(*RenderCommandFn)(void*);
		
		inline static const uint32_t BONE_SAMPLER_TEX_SLOT = 31;
		inline static const uint32_t BRDF_LUT_TEX_SLOT = 30;
		inline static const uint32_t ENVIRONMENT_CUBE_TEX_SLOT = 29;
		inline static const uint32_t ENVIRONMENT_IRRADIANCE_TEX_SLOT = 28;

		static void Init();
		static void Shutdown();

		static Ref<Texture2D> GetWhiteTexture();
		static Ref<Texture2D> GetDefaultBumpTexture();

		static void OnWindowResize(uint32_t width, uint32_t height);

		template<typename FuncT>
		static void Submit(FuncT&& func, const std::string& commandName)
		{

			if (
				true || 
				GetRenderCommandQueue().m_Deleted)
			{
				func();
				return;
			}

			auto renderCmd = [](void* ptr) {
				auto pFunc = (FuncT*)ptr;
				(*pFunc)();
				// NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
				// however some items like uniforms which contain std::strings still exist for now
				// static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
				pFunc->~FuncT();
			};
			auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func), commandName);
			new (storageBuffer) FuncT(std::forward<FuncT>(func));
		}

		static void Clear(float r, float g, float b, float a, bool clearColor, bool clearDepth, bool clearStencil);
		static void WaitAndRender();
		static void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest = true);
		// For OpenGL
		static void SetLineThickness(float thickness);


		// ~Actual~ Renderer here... TODO: remove confusion later
		static void BeginRenderPass(Ref<RenderPass> renderPass, bool clearColor, bool clearDepth, bool clearStencil);
		static void EndRenderPass();

		static void SubmitQuad(Ref<Material> material, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitQuad(Ref<Shader> shader, const glm::mat4& transform = glm::mat4(1.0f), bool depthTest = true);		
		static void SubmitFullscreenQuad(Ref<Material> material, bool depthTest);

		static void SubmitMesh(Ref<Shader> boundShader, Ref<Mesh> mesh, const glm::mat4& transform, const size_t& submeshIndex, Ref<Texture2D> boneMatrixTexture, const bool& depthTest);
		static void SubmitMesh(Ref<Shader> boundShader, Ref<Mesh> mesh, const glm::mat4& transform, Ref<Texture2D> boneMatrixTexture, const bool& depthTest);

		static void DrawAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color, bool depthTest);
		static void DrawAABB(Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec4& color, bool depthTest);

		static RenderCommandQueue& GetRenderCommandQueue();

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }


		struct RendererData
		{
			Ref<RenderPass> m_ActiveRenderPass;
			RenderCommandQueue m_CommandQueue;
			Ref<VertexArray> m_FullscreenQuadVertexArray;
			Ref<VertexArray> m_QuadVertexArray;

			Ref<Texture2D> m_WhiteTexture;
			Ref<Texture2D> m_DefaultBump;
		};

		static RendererData& GetRendererData();
	
	};
}
