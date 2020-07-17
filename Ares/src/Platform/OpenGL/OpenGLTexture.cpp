
#include "AresPCH.h"

#include "Ares/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include <stb_image.h>
//#include <glad/glad.h>

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



		auto self = this;

		Renderer::Submit([this]()
		{
				glGenTextures(1, &m_RendererID);
				glBindTexture(GL_TEXTURE_2D, m_RendererID);

				// if image is larger or smaller than actual size, what kind of filtering to use?
				// if shrinking image
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				// if enlarging image
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				GLenum wrap = m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
				glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

				glTexImage2D(GL_TEXTURE_2D, 0, Ares2OpenGLTextureFormat(m_Format), m_Width, m_Height, 0, Ares2OpenGLTextureFormat(m_Format));
				
				// gen mipmaps
				glGenerateMipmap(GL_TEXTURE_2D);
				// unbind
				glBindTexture(GL_TEXTURE_2D, 0);
		});
		// OLD ==================================================



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





		// NEW ====================================================
		int width, height, channels;
		if (stbi_is_hdr(path.c_str()))
		{
			ARES_CORE_INFO("Loading HDR Texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = (byte*)stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			m_IsHDR = true;
			m_Format = TextureFormat::Float16;
		}
		else
		{
			ARES_CORE_INFO("Loading Texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			ARES_CORE_ASSERT(m_ImageData.Data, "Could Not Read Image!");
			m_Format = TextureFormat::RGBA;
		}

		m_Width = width;
		m_Height = height;

		Renderer::Submit([=]() {
			// to do: consolidate properly
			if (srgb)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
				int levels = Texture::CalculateMipMapCount(m_Width, m_Height);
				ARES_CORE_INFO("Creating srgb texture with {0} mips", levels);
				glTextureStorage2D(m_RendererID, levels, GL_SRGB8, m_Width, m_Height);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData.Data);
				glGenerateTextureMipmap(m_RendererID);

			}
			else
			{
				glGenTextures(1, &m_RendererID);
				glBindTexture(GL_TEXTURE_2D, m_RendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				
				GLenum internalFormat = Ares2OpenGLTextureFormat(m_Format);
				GLenum format = srgb ? GL_SRGB8 : (m_IsHDR ? GL_RGB : Ares2OpenGLTextureFormat(m_Format)); //hdr
				GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;

				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, m_ImageData.Data);
				glGenerateMipmap(GL_TEXTURE_2D);


				glBindTexture(GL_TEXTURE_2D, 0);
			}
			stbi_image_free(m_ImageData.Data);
		});
	}

	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		//ARES_PROFILE_FUNCTION();

		Renderer::Submit([this]() {

			glDeleteTextures(1, &m_RendererID);
		});

	}
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		//ARES_PROFILE_FUNCTION();

		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		ARES_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Size in bytes must be entire texture!");

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		//ARES_PROFILE_FUNCTION();

		Renderer::Submit([this, slot]() {

			glBindTextureUnit(slot, m_RendererID);
		});

	}
	void OpenGLTexture2D::Lock()
	{
		m_Locked = true;
	}
	void OpenGLTexture2D::Unlock()
	{
		m_Locked = false;
	}
}