#pragma once


#include "Ares/Renderer/Texture.h"
#include <glad/glad.h>

namespace Ares {
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap);
		OpenGLTexture2D(const std::string& path, bool srgb);
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32_t slot = 0) const override;

		virtual TextureFormat GetFormat() const override { return m_Format; }
		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }


		virtual void Lock() override;
		virtual void Unlock() override;
		virtual Buffer GetWriteableBuffer() override;

		
		virtual const std::string& GetPath() const override { return m_Path; }

		inline virtual uint32_t GetRendererID() const override { return m_RendererID; }

		//virtual void SetData(void* data, uint32_t size) override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID;
		TextureFormat m_Format;
		TextureWrap m_Wrap = TextureWrap::Clamp;
		uint32_t m_Width, m_Height;

		Buffer m_ImageData;
		//bool m_IsHDR = false;
		bool m_Locked = false;

		std::string m_Path;
		//GLenum m_InternalFormat, m_DataFormat;
	};

}