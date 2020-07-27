
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
	Ref<TextureCube> TextureCube::Create(const std::string& path, bool srgb)
	{
		switch (Renderer::GetAPI())
		{

		case RendererAPI::API::None:
			ARES_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTextureCube>(path, srgb);
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

	void Texture2D::CalculateTilingAndOffsetForSubTexture(glm::vec2* tiling, glm::vec2* offset, const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		CalculateTilingAndOffsetForSubTexture(tiling, offset, texture->GetWidth(), texture->GetHeight(), coords, cellSize, spriteSize);
	}

	void Texture2D::CalculateTilingAndOffsetForSubTexture(glm::vec2* tiling, glm::vec2* offset, uint32_t width, uint32_t height, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		*tiling = { (spriteSize.x * cellSize.x) / width, (spriteSize.y * cellSize.y) / height };
		*offset = { (coords.x * cellSize.x) / width, (coords.y * cellSize.y) / height };
	}

	int Texture::CalculateMipMapCount(int width, int height)
	{
		int levels = 1;
		while ((width | height) >> levels) {
			levels++;
		}
		return levels;
	}

}