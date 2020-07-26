
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Ares/Renderer/Renderer.h"
#include <stb_image.h>

namespace Ares {

	static GLenum Ares2OpenGLTextureFormat(TextureFormat format)
	{
		//ARES_CORE_WARN(format);
		switch (format)
		{
		case TextureFormat::RGB: return GL_RGB;
		case TextureFormat::RGBA: return GL_RGBA;
		case TextureFormat::Float16: return GL_RGBA16F;

		}

		//ARES_CORE_ERROR(format);
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
		/*m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;*/

		Renderer::Submit([this]() mutable {
		//upload to opengl (gpu)
		//glCreateTextures(GL_TEXTURE_2D, 1, &this->m_RendererID);		
		//glTextureStorage2D(this->m_RendererID, 1, this->m_InternalFormat, this->m_Width, this->m_Height);
		//glTextureStorage2D(this->m_RendererID, 1, Ares2OpenGLTextureFormat(this->m_Format), this->m_Width, this->m_Height);

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

		//stbi_uc* data = nullptr;
		//data = stbi_load(path.c_str(), &width, &height, &channels, 0);

		m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		ARES_CORE_ASSERT(m_ImageData.Data, "Failed to load image!");
		
		m_Width = width;
		m_Height = height;

		m_Format = (channels == 4 ? TextureFormat::RGBA : TextureFormat::RGB);
		//ARES_CORE_ERROR(m_Format);

		// how opengl stores it
		/*GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}*/

		/*m_InternalFormat = internalFormat;
		m_DataFormat = dataFormat;*/

		//ARES_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		Renderer::Submit([this]() mutable {


			glGenTextures(1, &this->m_RendererID);
			glBindTexture(GL_TEXTURE_2D, this->m_RendererID);

			//upload to opengl (gpu)
			//glCreateTextures(GL_TEXTURE_2D, 1, &this->m_RendererID);
			//glTextureStorage2D(this->m_RendererID, 1, Ares2OpenGLTextureFormat(this->m_Format), m_Width, m_Height);
		
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

			//glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, this->m_Format == TextureFormat::RGB ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, this->m_ImageData.Data);
		
			// uploaded to gpu, now delete from cpu memory
			stbi_image_free(this->m_ImageData.Data);
		
		});

	}

	
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		Renderer::Submit([this]() {
		glDeleteTextures(1, &this->m_RendererID);
			});
		
	}
	/*void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bytesPerPixel = m_DataFormat == GL_RGBA ? 4 : 3;
		ARES_CORE_ASSERT(size == m_Width * m_Height * bytesPerPixel, "Size in bytes must be entire texture!");
		Renderer::Submit([this, data]() {
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
			});
	}*/
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

			//glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, m_DataFormat, GL_UNSIGNED_BYTE, this->m_ImageData.Data);

			glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, this->m_ImageData.Data);
		});
	}

	Buffer OpenGLTexture2D::GetWriteableBuffer()
	{
		ARES_CORE_ASSERT(m_Locked, "Texture must be locked!");
		return m_ImageData;
	}
}