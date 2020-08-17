#pragma once


#include "Ares/Renderer/Texture.h"
#include <glad/glad.h>

namespace Ares {

	
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, FilterType type, bool useMips);
		OpenGLTexture2D(const std::string& path, FilterType type, bool useMips, bool srgb);
		virtual ~OpenGLTexture2D();

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImmediate(uint32_t slot = 0) const override;

		virtual TextureFormat GetFormat() const override { return m_Format; }
		inline virtual uint32_t GetWidth() const override { return m_Width; }
		inline virtual uint32_t GetHeight() const override { return m_Height; }

		// This function currently returns the expected number of mips based on image size,
		// not present mips in data
		virtual uint32_t GetMipLevelCount() const override;

		virtual void SetData(void* data) override;

		virtual void Lock() override;
		virtual void Unlock() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		

		virtual Buffer GetWriteableBuffer() override;
		virtual bool Loaded() const override { return m_Loaded; }

		
		virtual const std::string& GetPath() const override { return m_Path; }

		inline virtual uint32_t GetRendererID() const override { return m_RendererID; }
		virtual void GenerateMipMaps() const override;

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

		virtual bool GetSRGB() const override { return m_SRGB; }

	private:
		uint32_t m_RendererID;
		TextureFormat m_Format;
		FilterType m_FilterType;
		TextureWrap m_Wrap = TextureWrap::Clamp;
		uint32_t m_Width, m_Height;

		Buffer m_ImageData;
		bool m_IsHDR = false;
		bool m_Locked = false;
		bool m_Loaded = false;
		bool m_SRGB = false;

		std::string m_Path;
	};



	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height, FilterType type, bool useMips);
		OpenGLTextureCube(const std::string& path, FilterType type, bool useMips);
		virtual ~OpenGLTextureCube();

		virtual void Bind(uint32_t slot = 0) const override;
		virtual void BindImmediate(uint32_t slot = 0) const override;

		virtual TextureFormat GetFormat() const { return m_Format; }
		virtual uint32_t GetWidth() const { return m_Width; }
		virtual uint32_t GetHeight() const { return m_Height; }
		
		// This function currently returns the expected number of mips based on image size,
		// not present mips in data
		virtual uint32_t GetMipLevelCount() const override;

		virtual const std::string& GetPath() const override { return m_FilePath; }

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
		}

		virtual bool GetSRGB() const override { return false; }

		virtual void GenerateMipMaps() const override;
	private:
		uint32_t m_RendererID;
		TextureFormat m_Format;
		FilterType m_FilterType;
		uint32_t m_Width, m_Height;

		unsigned char* m_ImageData;

		std::string m_FilePath;
	};
}