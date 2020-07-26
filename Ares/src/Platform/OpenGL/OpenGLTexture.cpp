
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Ares/Renderer/Renderer.h"
#include <stb_image.h>

namespace Ares {

	static GLenum Ares2OpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB: return GL_RGB;
		case TextureFormat::RGBA: return GL_RGBA;
		case TextureFormat::Float16: return GL_RGBA16F;
		}
		ARES_CORE_ASSERT(false, "Unknown Texture Format!");
		return 0;
	}

	/*
		TEXTURE 2D
	*/
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap)
		: m_Format(format), m_Width(width), m_Height(height), m_Wrap(wrap)
	{

		// how opengl stores it
		
		Renderer::Submit([this]() mutable {
			//upload to opengl (gpu)
		
			glGenTextures(1, &this->m_RendererID);
			glBindTexture(GL_TEXTURE_2D, this->m_RendererID);

			// if image is larger or smaller than actual size, what kind of filtering to use?
			// if shrinking image
			glTextureParameteri(this->m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// if enlarging image
			glTextureParameteri(this->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(this->m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(this->m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D, 0, Ares2OpenGLTextureFormat(this->m_Format), this->m_Width, this->m_Height, 0, Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, nullptr);
		
		});

		m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb)
		: m_Path(path), m_Format(TextureFormat::RGB), m_Width(1), m_Height(1)
	{

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		ARES_CORE_ASSERT(m_ImageData.Data, "Failed to load image!");
		
		m_Width = width;
		m_Height = height;

		m_Format = (channels == 4 ? TextureFormat::RGBA : TextureFormat::RGB);
		
		Renderer::Submit([this]() mutable {

			//upload to opengl (gpu)
			glGenTextures(1, &this->m_RendererID);
			glBindTexture(GL_TEXTURE_2D, this->m_RendererID);

			// if image is larger or smaller than actual size, what kind of filtering to use?
			// if shrinking image
			glTextureParameteri(this->m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// if enlarging image
			glTextureParameteri(this->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(this->m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(this->m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			
			GLenum internalFormat = Ares2OpenGLTextureFormat(this->m_Format);
			GLenum format = Ares2OpenGLTextureFormat(this->m_Format);
			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, this->m_Width, this->m_Height, 0, format, type, this->m_ImageData.Data);

			// uploaded to gpu, now delete from cpu memory
			stbi_image_free(this->m_ImageData.Data);
		
		});

	}

	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
		});
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		Renderer::Submit([this, slot]() {
			glBindTextureUnit(slot, this->m_RendererID);
		});
	}


	void OpenGLTexture2D::Lock()
	{
		m_Locked = true;
	}

	void OpenGLTexture2D::Unlock()
	{
		m_Locked = false;
		Renderer::Submit([this]() {
			glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, this->m_ImageData.Data);
		});
	}

	Buffer OpenGLTexture2D::GetWriteableBuffer()
	{
		ARES_CORE_ASSERT(m_Locked, "Texture must be locked!");
		return m_ImageData;
	}
}