
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLTexture.h"
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
		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		//upload to opengl (gpu)
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		// if image is larger or smaller than actual size, what kind of filtering to use?
		// if shrinking image
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// if enlarging image
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb)
		: m_Path(path)
	{
		ARES_PROFILE_FUNCTION();

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = nullptr;
		{
			ARES_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string& path)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		ARES_CORE_ASSERT(data, "Failed to load image!");
		
		m_Width = width;
		m_Height = height;

		// how opengl stores it
		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;

		ARES_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		//upload to opengl (gpu)
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
		
		// if image is larger or smaller than actual size, what kind of filtering to use?
		// if shrinking image
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// if enlarging image
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);


		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		// uploaded to gpu, now delete from cpu memory
		stbi_image_free(data);

	}

	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
		
	}
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		ARES_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Size in bytes must be entire texture!");

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}