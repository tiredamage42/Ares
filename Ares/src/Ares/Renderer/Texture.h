#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Buffer.h"
//#include <string>

namespace Ares {
	enum class TextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2,
		Float16 = 3,
	};
	enum class TextureWrap
	{
		None = 0,
		Clamp = 1,
		Repeat = 2
	};
	class Texture
	{
	public:
		virtual ~Texture() = default;
		
		virtual void Bind(uint32_t slot = 0) const = 0;
		
		virtual uint32_t GetRendererID() const = 0;
		
		//virtual void SetData(void* data, uint32_t size) = 0;
		static uint32_t GetBPP(TextureFormat format);


		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap);
		static Ref<Texture2D> Create(const std::string& path, bool srgb = false);

		virtual TextureFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;

		virtual Buffer GetWriteableBuffer() = 0;

		virtual const std::string& GetPath() const = 0;
	};
	
}