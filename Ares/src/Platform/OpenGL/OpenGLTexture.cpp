
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			// if enlarging image
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			GLenum wrap = this->m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
			glTextureParameterf(this->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

			glTexImage2D(GL_TEXTURE_2D, 0, Ares2OpenGLTextureFormat(this->m_Format), this->m_Width, this->m_Height, 0, Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, nullptr);
			
			//glGenerateMipmap(GL_TEXTURE_2D);

			// unbind
			glBindTexture(GL_TEXTURE_2D, 0);
		});

		m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb)
		: m_Path(path), m_Format(TextureFormat::RGB), m_Width(1), m_Height(1)
	{

		ARES_CORE_INFO("Loading texture {0}, srgb={1}", path, srgb);
		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;


		if (stbi_is_hdr(path.c_str()))
		{
			ARES_CORE_INFO("Loading HDR texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = (byte*)stbi_loadf(path.c_str(), &width, &height, &channels, 0);
			m_Format = TextureFormat::Float16;
			m_IsHDR = true;
		}
		else
		{
			ARES_CORE_INFO("Loading texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			m_Format = (srgb ? TextureFormat::RGB : TextureFormat::RGBA);
		}
		ARES_CORE_ASSERT(m_ImageData.Data, "Failed to load image!");

		m_Loaded = true;
		m_Width = width;
		m_Height = height;

		
		Renderer::Submit([this, srgb]() mutable {

			//upload to opengl (gpu)
			//glGenTextures(1, &this->m_RendererID);
			//glBindTexture(GL_TEXTURE_2D, this->m_RendererID);

			//int levels = CalculateMipMapCount(this->m_Width, this->m_Height);
			//ARES_CORE_INFO("Creating texture with {0} mips", levels);

			//// if image is larger or smaller than actual size, what kind of filtering to use?
			//// if shrinking image
			//glTextureParameteri(this->m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//// if enlarging image
			//glTextureParameteri(this->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			//glTextureParameteri(this->m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			//glTextureParameteri(this->m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//
			//GLenum internalFormat = Ares2OpenGLTextureFormat(this->m_Format);
			//GLenum format = Ares2OpenGLTextureFormat(this->m_Format);
			//GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
			//glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, this->m_Width, this->m_Height, 0, format, type, this->m_ImageData.Data);



			// TODO: Consolidate properly
			if (srgb)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &this->m_RendererID);
				int levels = CalculateMipMapCount(this->m_Width, this->m_Height);
				ARES_CORE_INFO("Creating srgb texture with {0} mips", levels);
				glTextureStorage2D(this->m_RendererID, levels, GL_SRGB8, this->m_Width, this->m_Height);
				glTextureParameteri(this->m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTextureParameteri(this->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, GL_RGB, GL_UNSIGNED_BYTE, this->m_ImageData.Data);
				glGenerateTextureMipmap(this->m_RendererID);
			}
			else
			{
				glGenTextures(1, &this->m_RendererID);
				glBindTexture(GL_TEXTURE_2D, this->m_RendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				GLenum internalFormat = Ares2OpenGLTextureFormat(m_Format);
				GLenum format = srgb ? GL_SRGB8 : (m_IsHDR ? GL_RGB : Ares2OpenGLTextureFormat(m_Format)); // HDR = GL_RGB for now
				GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, m_ImageData.Data);
				//glTexImage2D(GL_TEXTURE_2D, 0, Ares2OpenGLTextureFormat(this->m_Format), this->m_Width, this->m_Height, 0, srgb ? GL_SRGB8 : Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, this->m_ImageData.Data);

				glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

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

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		ARES_CORE_ASSERT(m_Locked, "Texture must be locked!");

		m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));
#if HZ_DEBUG
		m_ImageData.ZeroInitialize();
#endif
	}

	Buffer OpenGLTexture2D::GetWriteableBuffer()
	{
		ARES_CORE_ASSERT(m_Locked, "Texture must be locked!");
		return m_ImageData;
	}
	uint32_t OpenGLTexture2D::GetMipLevelCount() const
	{
		return CalculateMipMapCount(m_Width, m_Height);
	}






	OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;

		uint32_t levels = CalculateMipMapCount(width, height);

		Renderer::Submit([=]() {
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, levels, Ares2OpenGLTextureFormat(m_Format), width, height);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			// glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, 16);
		});
	}

	// TODO: Revisit this, as currently env maps are being loaded as equirectangular 2D images
	//       so this is an old path

	OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
		: m_FilePath(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		m_ImageData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

		m_Width = width;
		m_Height = height;
		m_Format = TextureFormat::RGB;

		/*
			  x
			x x x x    Format
 			  x
		*/
		uint32_t faceWidth = m_Width / 4;
		uint32_t faceHeight = m_Height / 3;
		ARES_CORE_ASSERT(faceWidth == faceHeight, "Non-square faces!");

		const uint32_t bytesPerPixel = 3;
		std::array<uint8_t*, 6> faces;
		for (size_t i = 0; i < faces.size(); i++)
			faces[i] = new uint8_t[faceWidth * faceHeight * bytesPerPixel]; // 3 BPP

		int faceIndex = 0;

		for (size_t i = 0; i < 4; i++)
		{
			for (size_t y = 0; y < faceHeight; y++)
			{
				size_t yOffset = y + faceHeight;
				for (size_t x = 0; x < faceWidth; x++)
				{
					size_t xOffset = x + i * faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		for (size_t i = 0; i < 3; i++)
		{
			// Skip the middle one
			if (i == 1)
				continue;

			for (size_t y = 0; y < faceHeight; y++)
			{
				size_t yOffset = y + i * faceHeight;
				for (size_t x = 0; x < faceWidth; x++)
				{
					size_t xOffset = x + faceWidth;
					faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
				}
			}
			faceIndex++;
		}

		Renderer::Submit([this, faceWidth, faceHeight, faces]() {
			glGenTextures(1, &this->m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_RendererID);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			glTextureParameterf(this->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

			auto format = Ares2OpenGLTextureFormat(this->m_Format);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			glBindTexture(GL_TEXTURE_2D, 0);

			for (size_t i = 0; i < faces.size(); i++)
				delete[] faces[i];

			stbi_image_free(this->m_ImageData);
		});
	}
	OpenGLTextureCube::~OpenGLTextureCube()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
		});
	}
	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		Renderer::Submit([this, slot]() {
			glBindTextureUnit(slot, this->m_RendererID);
			/*glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_RendererID);*/
		});
	}
	uint32_t OpenGLTextureCube::GetMipLevelCount() const
	{
		return CalculateMipMapCount(m_Width, m_Height);
	}
}