#pragma once
#include <Ares.h>

//#include "ParticleSystem.h"
//#include "Platform/OpenGL/OpenGLShader.h"
//#include "imgui/imgui.h"
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

namespace Ares
{

	class EditorLayer : public Layer
	{
	public:

		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnImGuiDraw() override;
		virtual void OnEvent(Event& e) override;


	private:
		OrthographicCameraController m_CameraController;
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Texture2D> m_Texture;
		Ref<FrameBuffer> m_FrameBuffer;
		//Ref<Texture2D> m_SpriteSheet;
		//Ref<SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

		struct ProfileResult
		{
			const char* Name;
			float Time;
		};

		glm::vec4 m_SquareColor = { .2f, .3f, .8f, 1.0f };

		/*ParticleSystem m_ParticleSystem;
		ParticleProps m_Particle;*/


		/*uint32_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Ares::Ref<Ares::SubTexture2D>> s_TextureMap;*/
	};
}
