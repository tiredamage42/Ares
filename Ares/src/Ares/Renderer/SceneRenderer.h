#pragma once
#include "Ares/Core/Scene.h"
#include "Ares/Renderer/Mesh.h"
#include "RenderPass.h"

namespace Ares {
	struct SceneRendererOptions
	{
		Vector4 MeshTrianglesColor{ 51.0f / 255.0f, 105.0f / 255.0f, 173.0f / 255.0f, 1.0f };
		Vector4 AABBColor{ 0, 1, 0, 1.0f };

		Vector4 OutlineColor{ 1, .5f, 0, 1 };
		bool ShowGrid = true;
		bool ShowBoundingBoxes = false;
		Vector4 GridColor{ .75f, .75f, .75f, 50.0f / 255.0f };
		int GridResolution = 100;
		Vector2 GridCameraRange{ 0, 75 };

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
		static void Shutdown();

		static void SetViewportSize(uint32_t width, uint32_t height);

		static void BeginScene(const Scene* scene, const SceneRendererCamera& camera);
		static void EndScene();
		static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<Texture2D> boneTransforms, std::vector<Ref<Material>> materials, bool isSelected);
		static Ref<TextureCube> ConvertHDRToCubemap(const std::string& filepath, uint32_t cubemapSize = 2048);

		static Ref<RenderPass> GetFinalRenderPass();
		static Ref<RenderPass> GetGeometryPass();

		// TODO: Temp
		static uint32_t GetFinalColorBufferRendererID();

		static SceneRendererOptions& GetOptions();
	private:
		static void FlushDrawList();
		static void GeometryPass();
		static void CompositePass();
	};

}