#pragma once
#include "Ares/Core/Scene.h"
#include "Ares/Renderer/Mesh.h"
#include "RenderPass.h"

namespace Ares {
	struct SceneRendererOptions
	{
		bool ShowGrid = true;
		bool ShowBoundingBoxes = false;
	};

	struct SceneRendererCamera
	{
		Ares::Camera Camera;
		glm::mat4 ViewMatrix;
	};



	class SceneRenderer
	{
	public:
		static void Init();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene, const SceneRendererCamera& camera);
		static void EndScene();

		/*static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> materialOverride);
		static void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> materialOverride);*/
		
		static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, std::vector<Ref<Material>> materials, const std::string& name);
		static void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform, std::vector<Ref<Material>> materials);

		static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

		//static Ref<Texture2D> GetFinalColorBuffer();
		static Ref<RenderPass> GetFinalRenderPass();

		// TODO: Temp
		static uint32_t GetFinalColorBufferRendererID();

		static SceneRendererOptions& GetOptions();
	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();
	};

}