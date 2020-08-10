#include "AresPCH.h"
#include "Ares/Renderer/Renderer.h"
#include "Ares/Renderer/Renderer2D.h"

#include <glad/glad.h>
#include "SceneRenderer.h"


namespace Ares {

	Scope<Renderer::SceneData> Renderer::s_SceneData = CreateScope<Renderer::SceneData>();
	
	/*RenderCommandQueue Renderer::s_CommandQueue;
	Ref<RenderPass> Renderer::s_ActiveRenderPass;*/


	struct RendererData
	{
		Ref<RenderPass> m_ActiveRenderPass;
		RenderCommandQueue m_CommandQueue;
		Ref<VertexArray> m_FullscreenQuadVertexArray;
	};

	static RendererData s_Data;

	RenderCommandQueue& Renderer::GetRenderCommandQueue()
	{
		return s_Data.m_CommandQueue;
	}
	void Renderer::Init()
	{
		Submit([]() { RenderCommand::Init(); });
		Renderer2D::Init();

		SceneRenderer::Init();

		// Create Quad
		float* data = new float[4 * 2]{
			-1, -1,
			 1, -1,
			 1,  1,
			-1,  1
		};

		s_Data.m_FullscreenQuadVertexArray = VertexArray::Create();

		Ref<VertexBuffer> quadVB = VertexBuffer::Create(data, 4 * 2 * sizeof(float));
		quadVB->SetLayout({
			{ ShaderDataType::Float2, "a_Position" },
		});
		s_Data.m_FullscreenQuadVertexArray->AddVertexBuffer(quadVB);

		uint32_t* quadIndicies = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
		s_Data.m_FullscreenQuadVertexArray->SetIndexBuffer(IndexBuffer::Create(quadIndicies, 6));
		delete[] quadIndicies;
		delete[] data;

	}
	
	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		Submit([=]() {

			RenderCommand::DrawIndexed(count, type, depthTest);
		});
	}
	void Renderer::SetLineThickness(float thickness)
	{
		Renderer::Submit([=]() {
			RenderCommand::SetLineThickness(thickness);
		});
	}

	void Renderer::BeginRenderPass(Ref<RenderPass> renderPass, bool clear)
	{
		ARES_CORE_ASSERT(renderPass, "Render pass cannot be null!");

		// TODO: Convert all of this into a render command buffer
		s_Data.m_ActiveRenderPass = renderPass;
		renderPass->GetSpecs().TargetFrameBuffer->Bind();

		if (clear)
		{
			const glm::vec4& clearColor = renderPass->GetSpecs().TargetFrameBuffer->GetSpecs().ClearColor;
			Renderer::Submit([=]() {
				RenderCommand::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			});
		}
	}

	void Renderer::EndRenderPass()
	{
		ARES_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
		s_Data.m_ActiveRenderPass->GetSpecs().TargetFrameBuffer->Unbind();
		s_Data.m_ActiveRenderPass = nullptr;
	}

	void Renderer::SubmitQuad(Ref<MaterialInstance> material, const glm::mat4& transform)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);
			//auto shader = material->GetShader();
			material->GetShader()->SetMat4("u_Transform", transform);
		}

		s_Data.m_FullscreenQuadVertexArray->Bind();
		DrawIndexed(6, PrimitiveType::Triangles, depthTest);
	}

	void Renderer::SubmitFullscreenQuad(Ref<MaterialInstance> material)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);
		}

		s_Data.m_FullscreenQuadVertexArray->Bind();
		DrawIndexed(6, PrimitiveType::Triangles, depthTest);
	}
	void Renderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
	{
		// TODO: Sort this out
		mesh->m_VertexArray->Bind();

		auto& materialOverrides = mesh->GetMaterialOverrides();


		for (Submesh& submesh : mesh->m_Submeshes)
		{
			// Material

			auto material = overrideMaterial ? overrideMaterial : materialOverrides[submesh.MaterialIndex];
			//auto material = materialOverrides[submesh.MaterialIndex];

			auto shader = material->GetShader();
			material->Bind();

			if (mesh->m_IsAnimated)
			{
				for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
					shader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
				}
			}
			shader->SetMat4("u_Transform", transform * submesh.Transform);

			Renderer::Submit([submesh, material]() {
				if (material->GetFlag(MaterialFlag::DepthTest))
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
			});
		}

		// TODO: replace with render API calls
		/*Renderer::Submit([=]()
		{
			for (Submesh& submesh : mesh->m_Submeshes)
			{
				if (mesh->m_IsAnimated)
				{
					for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
					{
						std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
						shader->SetMat4FromRenderThread(uniformName, mesh->m_BoneTransforms[i]);
					}
				}

				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
			}
		});*/
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		Submit([=]() { RenderCommand::SetViewport(0, 0, width, height); });
	}

	void Renderer::Clear(float r, float g, float b, float a)
	{
		Submit([=]() { RenderCommand::Clear(r, g, b, a); });
	}	
	void Renderer::WaitAndRender()
	{

		s_Data.m_CommandQueue.Execute();
	}

	void Renderer::DrawAABB(Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec4& color)
	{
		for (Submesh& submesh : mesh->m_Submeshes)
		{
			auto& aabb = submesh.BoundingBox;
			auto aabbTransform = transform * submesh.Transform;
			DrawAABB(aabb, aabbTransform, color);
		}
	}

	void Renderer::DrawAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color /*= glm::vec4(1.0f)*/)

	{
		glm::vec4 min = { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f };
		glm::vec4 max = { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f };

		/*for (Submesh& submesh : mesh->m_Submeshes)
		{
			const auto& transform = submesh.Transform;
			glm::vec4 min = { submesh.Min.x, submesh.Min.y, submesh.Min.z, 1.0f };
			glm::vec4 max = { submesh.Max.x, submesh.Max.y, submesh.Max.z, 1.0f };*/

			glm::vec4 corners[8] =
			{
				transform * glm::vec4 { aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f },
				transform * glm::vec4 { aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f },
				transform * glm::vec4 { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f },
				transform * glm::vec4 { aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f },
													
				transform * glm::vec4 { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f },
				transform * glm::vec4 { aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f },
				transform * glm::vec4 { aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f },
				transform * glm::vec4 { aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f }
			};

			for (uint32_t i = 0; i < 4; i++)
				Renderer2D::DrawLine(corners[i], corners[(i + 1) % 4], color);

			for (uint32_t i = 0; i < 4; i++)
				Renderer2D::DrawLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

			for (uint32_t i = 0; i < 4; i++)
				Renderer2D::DrawLine(corners[i], corners[i + 4], color);
		//}
	}
}