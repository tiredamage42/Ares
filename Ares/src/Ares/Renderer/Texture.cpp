
#include "AresPCH.h"
#include "Ares/Renderer/Texture.h"
#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Ares 
{
	Ref<Texture2D> Texture2D::Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(format, width, height, wrap);
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(const std::string& path, bool srgb)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path, srgb);
		}

		ARES_CORE_ASSERT(false, "Unknow RendererAPI");
		return nullptr;
	}


	uint32_t Texture::GetBPP(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB:    return 3;
		case TextureFormat::RGBA:   return 4;
		}
		return 0;
	}




}