
#include "AresPCH.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Ares/Renderer/Renderer.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ares {


	/*
	GL_NEAREST - no filtering, no mipmaps
	GL_LINEAR - filtering, no mipmaps
	GL_NEAREST_MIPMAP_NEAREST - no filtering, sharp switching between mipmaps
	GL_NEAREST_MIPMAP_LINEAR - no filtering, smooth transition between mipmaps
	GL_LINEAR_MIPMAP_NEAREST - filtering, sharp switching between mipmaps
	GL_LINEAR_MIPMAP_LINEAR - filtering, smooth transition between mipmaps

	So:
	GL_LINEAR is bilinear
	GL_LINEAR_MIPMAP_NEAREST is bilinear with mipmaps
	GL_LINEAR_MIPMAP_LINEAR is trilinear
	*/

	static GLint Ares2OpenGLMinFiltering(FilterType filterType, uint8_t levels)
	{

		
		if (levels <= 1)
			return filterType == FilterType::Point ? GL_NEAREST : GL_LINEAR;
		
		switch (filterType)
		{
			case FilterType::Point: return GL_NEAREST_MIPMAP_NEAREST;
			case FilterType::Bilinear: return GL_LINEAR_MIPMAP_NEAREST;
			case FilterType::Trilinear: return GL_LINEAR_MIPMAP_LINEAR;
		}
		ARES_CORE_ASSERT(false, "Unknown Filter Type!");
		return 0;
	}
	static GLint Ares2OpenGLMagFiltering(FilterType filterType)
	{
		return filterType == FilterType::Point ? GL_NEAREST : GL_LINEAR;
	}

	static GLenum Ares2OpenGLInternalTextureFormat(TextureFormat format)
	{

		switch (format)
		{
			case TextureFormat::RGB:		return GL_RGB8;
			case TextureFormat::RGBA:		return GL_RGBA8;
			case TextureFormat::Float16:	return GL_RGBA16F;
			case TextureFormat::RG16:		return GL_RG16F;
		}
		ARES_CORE_ASSERT(false, "Unknown texture format!");
		return 0;
	}

	static GLenum Ares2OpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::RGB: return GL_RGB;
		case TextureFormat::RGBA: return GL_RGBA;
		case TextureFormat::Float16: return GL_RGBA;
		case TextureFormat::RG16: return GL_RG;
		}
		ARES_CORE_ASSERT(false, "Unknown Texture Format!");
		return 0;
	}

	/*
		TEXTURE 2D
	*/
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, FilterType filterType, bool useMips)
		: m_Format(format), m_Width(width), m_Height(height), m_Wrap(wrap), m_FilterType(filterType)
	{

		// how opengl stores it
		
		GLenum _wrap = this->m_Wrap == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;

		int levels = !useMips ? 1 : CalculateMipMapCount(this->m_Width, this->m_Height);
		GLint minFilter = Ares2OpenGLMinFiltering(filterType, levels);
		GLint maxFilter = Ares2OpenGLMagFiltering(filterType);

		Renderer::Submit([=]() mutable {
			//upload to opengl (gpu)
			glGenTextures(1, &this->m_RendererID);
			glBindTexture(GL_TEXTURE_2D, this->m_RendererID);
			//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);

			//glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
			//glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, maxFilter);


			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap);
			/*glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, _wrap);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, _wrap);*/


			glTextureParameterf(this->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

			glTexImage2D(GL_TEXTURE_2D, 0, Ares2OpenGLInternalTextureFormat(this->m_Format), this->m_Width, this->m_Height, 0, Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, nullptr);
			
			//glGenerateMipmap(GL_TEXTURE_2D);

			// unbind
			glBindTexture(GL_TEXTURE_2D, 0);
			//glTextureStorage2D(m_RendererID, levels, Ares2OpenGLInternalTextureFormat(m_Format), m_Width, m_Height);
		}, "Create Texture Empty");

		//m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));

	}

	
	
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, FilterType filterType, bool useMips, bool srgb)
		: m_Path(path), m_Format(TextureFormat::RGB), m_Width(1), m_Height(1), m_FilterType(filterType)
	{

		//ARES_CORE_INFO("Loading texture {0}, srgb={1}", path, srgb);
		stbi_set_flip_vertically_on_load(1);

		int width, height, channels;

		//void* data = nullptr;
		if (stbi_is_hdr(path.c_str()))
		{

			ARES_CORE_INFO("Loading HDR texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = (byte*)
				//data = 
				stbi_loadf(path.c_str(), &width, &height, &channels, STBI_rgb);
			m_Format = TextureFormat::Float16;
			m_IsHDR = true;
		}
		else
		{
			ARES_CORE_INFO("Loading texture {0}, srgb={1}", path, srgb);
			m_ImageData.Data = (byte*)
				//data = 
				stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
			m_Format = (srgb ? TextureFormat::RGB : TextureFormat::RGBA);
		}
		ARES_CORE_ASSERT(m_ImageData.Data, "Failed to load image!");
		//ARES_CORE_ASSERT(data, "Failed to load image!");

		m_Loaded = true;
		m_Width = width;
		m_Height = height;
		m_SRGB = srgb;

		
		Renderer::Submit([=]() mutable {

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

			std::string p = path;
 			int levels = !useMips ? 1 : CalculateMipMapCount(m_Width, m_Height);
			GLint minFilter = Ares2OpenGLMinFiltering(filterType, levels);
			GLint maxFilter = Ares2OpenGLMagFiltering(filterType);


			// TODO: Consolidate properly
			if (srgb)
			{
				glGenTextures(1, &this->m_RendererID);
				glBindTexture(GL_TEXTURE_2D, this->m_RendererID);

				//glCreateTextures(GL_TEXTURE_2D, 1, &this->m_RendererID);
				//int levels = CalculateMipMapCount(this->m_Width, this->m_Height);
				
				//ARES_CORE_INFO("Creating srgb texture with {0} mips", levels);
				//glTextureStorage2D(this->m_RendererID, levels, GL_SRGB8, this->m_Width, this->m_Height);
				
				//glTextureParameteri(this->m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
				//glTextureParameteri(this->m_RendererID, GL_TEXTURE_MAG_FILTER, maxFilter);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);



				glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_ImageData.Data);

				//glTextureStorage2D(this->m_RendererID, levels, GL_SRGB8, this->m_Width, this->m_Height);
				//glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, GL_RGB, GL_UNSIGNED_BYTE, this->m_ImageData.Data);

				if (useMips)
				{

					//glGenerateTextureMipmap(this->m_RendererID);
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			else
			{
				glGenTextures(1, &this->m_RendererID);
				glBindTexture(GL_TEXTURE_2D, this->m_RendererID);
				//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				/*glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, maxFilter);
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);*/


				GLenum internalFormat = Ares2OpenGLInternalTextureFormat(m_Format);
				//GLenum internalFormat = srgb ? GL_SRGB8 : Ares2OpenGLInternalTextureFormat(m_Format);

				//GLenum format = srgb ? GL_SRGB8 : (m_IsHDR ? GL_RGB : Ares2OpenGLTextureFormat(m_Format)); // HDR = GL_RGB for now
				GLenum format = (m_IsHDR ? GL_RGB : Ares2OpenGLTextureFormat(m_Format)); // HDR = GL_RGB for now

				GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;


				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, m_ImageData.Data);
				////glTexImage2D(GL_TEXTURE_2D, 0, Ares2OpenGLTextureFormat(this->m_Format), this->m_Width, this->m_Height, 0, srgb ? GL_SRGB8 : Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, this->m_ImageData.Data);

				//glGenerateMipmap(GL_TEXTURE_2D);


				//glTextureStorage2D(m_RendererID, levels, internalFormat, m_Width, m_Height);
				//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, type, data);// m_ImageData.Data);

				if (useMips)
				{
					//glGenerateTextureMipmap(m_RendererID);
					glGenerateMipmap(GL_TEXTURE_2D);
				}

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// uploaded to gpu, now delete from cpu memory
			stbi_image_free( m_ImageData.Data);
		
		}, "Create Texture Load " + path);

	}

	
	OpenGLTexture2D::~OpenGLTexture2D()
	{

		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
		}, "Delete Texture");
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		Renderer::Submit([this, slot]() {
			glBindTextureUnit(slot, this->m_RendererID);
		}, "Texture Bind");
	}


	void OpenGLTexture2D::Lock()
	{
		m_Locked = true;
		m_ImageData.Allocate(m_Width * m_Height * Texture::GetBPP(m_Format));
	}

	void OpenGLTexture2D::SetData(void* data)
	{

		Renderer::Submit([=]() {

			GLenum internalFormat = Ares2OpenGLInternalTextureFormat(m_Format);
			GLenum format = Ares2OpenGLTextureFormat(m_Format);
			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;

			/*glBindTexture(GL_TEXTURE_2D, m_RendererID);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, format, type, data);*/
			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, format, type, data);

		}, "Texture Reset Data");
	}

	/*void OpenGLTexture2D::SetData(void* data)
	{
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, Ares2OpenGLTextureFormat(m_Format), GL_UNSIGNED_BYTE, data);
	}*/

	void OpenGLTexture2D::Unlock()
	{
		m_Locked = false;
		Renderer::Submit([this]() {
			glTextureSubImage2D(this->m_RendererID, 0, 0, 0, this->m_Width, this->m_Height, Ares2OpenGLTextureFormat(this->m_Format), GL_UNSIGNED_BYTE, this->m_ImageData.Data);
		}, "Texture Reset Data");
	}

	void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
	{
		ARES_CORE_ASSERT(m_Locked, "Texture must be locked!");

		m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));
#if ARES_DEBUG
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

	void OpenGLTexture2D::GenerateMipMaps() const
	{
		Renderer::Submit([=]() {
			glGenerateTextureMipmap(m_RendererID);
		}, "Texture GenMipmaps");
	}






	OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height, FilterType filterType, bool useMips)
	{
		m_Width = width;
		m_Height = height;
		m_Format = format;

		uint32_t levels = !useMips ? 1 : CalculateMipMapCount(width, height);

		GLint minFilter = Ares2OpenGLMinFiltering(filterType, levels);
		GLint maxFilter = Ares2OpenGLMagFiltering(filterType);


		Renderer::Submit([=]() {



			glGenTextures(1, &m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

			GLenum internalFormat = Ares2OpenGLInternalTextureFormat(m_Format);
			GLenum format = Ares2OpenGLTextureFormat(m_Format); // HDR = GL_RGB for now
			GLenum type = internalFormat == GL_RGBA16F ? GL_FLOAT : GL_UNSIGNED_BYTE;


			for (uint32_t i = 0; i < 6; ++i)
			{
				// from pbr tutorial:
				//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, type, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, maxFilter);

			if (useMips)
			{
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
				//glGenerateTextureMipmap(m_RendererID);
			}


			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			/*
			glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, levels, Ares2OpenGLInternalTextureFormat(m_Format), width, height);
			//glTextureStorage2D(m_RendererID, levels, Ares2OpenGLTextureFormat(m_Format), width, height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, maxFilter);
			
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			*/




			/*glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);*/

			// glTextureParameterf(m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, 16);

			//glTextureStorage2D(m_RendererID, levels, Ares2OpenGLInternalTextureFormat(m_Format), width, height);
		}, "Create Cube empty");
	}

	// TODO: Revisit this, as currently env maps are being loaded as equirectangular 2D images
	//       so this is an old path

	OpenGLTextureCube::OpenGLTextureCube(const std::string& path, FilterType filterType, bool useMips)
		: m_FilePath(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
			
		m_ImageData = 
		//stbi_uc* data =	
			stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

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
					/*faces[faceIndex][(x + y * faceWidth) * 3 + 0] = data[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = data[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = data[(xOffset + yOffset * m_Width) * 3 + 2];*/
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
					/*faces[faceIndex][(x + y * faceWidth) * 3 + 0] = data[(xOffset + yOffset * m_Width) * 3 + 0];
					faces[faceIndex][(x + y * faceWidth) * 3 + 1] = data[(xOffset + yOffset * m_Width) * 3 + 1];
					faces[faceIndex][(x + y * faceWidth) * 3 + 2] = data[(xOffset + yOffset * m_Width) * 3 + 2];*/
				}
			}
			faceIndex++;
		}




		Renderer::Submit([=]() {
			uint32_t levels = !useMips ? 1 : CalculateMipMapCount(width, height);
			GLint minFilter = Ares2OpenGLMinFiltering(filterType, levels);
			GLint maxFilter = Ares2OpenGLMagFiltering(filterType);


			glGenTextures(1, &this->m_RendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_RendererID);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, maxFilter);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			/*glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_RendererID);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, maxFilter);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);*/



			glTextureParameterf(this->m_RendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

			auto internalFormat = Ares2OpenGLInternalTextureFormat(m_Format);
			auto format = Ares2OpenGLTextureFormat(this->m_Format);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);
															
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);
															
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

			//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			// maybe 3D?
			/*glTextureStorage2D(m_RendererID, levels, format, faceWidth, faceHeight);
			for (uint32_t i = 0; i < 6; i++)
				glTextureSubImage3D(m_RendererID, 0, 0, 0, i, faceWidth, faceHeight, 1, format, GL_UNSIGNED_BYTE, faces[i]);*/



			if (useMips)
			{
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
				//glGenerateTextureMipmap(m_RendererID);
			}
			
			glBindTexture(GL_TEXTURE_2D, 0);

			for (size_t i = 0; i < faces.size(); i++)
				delete[] faces[i];

			stbi_image_free(this->m_ImageData);
			//stbi_image_free(data);
		}, "Create cube loaded");
	}
	OpenGLTextureCube::~OpenGLTextureCube()
	{
		GLuint rendererID = m_RendererID;
		Renderer::Submit([rendererID]() {
			glDeleteTextures(1, &rendererID);
		}, "Delete cube");
	}
	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		Renderer::Submit([this, slot]() {
			//glBindTextureUnit(slot, m_RendererID);
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_RendererID);
		}, "Bind Cube");
	}
	uint32_t OpenGLTextureCube::GetMipLevelCount() const
	{
		return CalculateMipMapCount(m_Width, m_Height);
	}
	void OpenGLTextureCube::GenerateMipMaps() const
	{
		Renderer::Submit([this]() {
			glGenerateTextureMipmap(m_RendererID);
		}, "Cube gen mipmaps");
	}

}