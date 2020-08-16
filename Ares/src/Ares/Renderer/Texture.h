#pragma once

#include "Ares/Core/Core.h"
#include "Ares/Core/Buffer.h"
#include <glm/glm.hpp>

namespace Ares {
	enum class FilterType
	{
		Point, Bilinear, Trilinear
	};
	
	enum class TextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2,
		Float16 = 3,
		RG16 = 4
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
		virtual bool operator==(const Texture& other) const = 0;

		virtual TextureFormat GetFormat() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual bool GetSRGB() const = 0;

		virtual uint32_t GetMipLevelCount() const = 0;

		static uint32_t GetBPP(TextureFormat format);
		static int CalculateMipMapCount(int width, int height);
		virtual void GenerateMipMaps() const = 0;

		virtual const std::string& GetPath() const = 0;

	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, FilterType filterType, bool useMips);
		static Ref<Texture2D> Create(const std::string& path, FilterType filterType, bool useMips, bool srgb = false);

		static void CalculateTilingAndOffsetForSubTexture(glm::vec2* tiling, glm::vec2* offset, const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 });
		static void CalculateTilingAndOffsetForSubTexture(glm::vec2* tiling, glm::vec2* offset, uint32_t width, uint32_t height, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = { 1, 1 });

		//virtual void SetData(void* data) = 0;

		virtual void Lock() = 0;
		virtual void Unlock() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual Buffer GetWriteableBuffer() = 0;

		virtual void SetData(void* data) = 0;

		virtual bool Loaded() const = 0;


		//virtual const std::string& GetPath() const = 0;
	};

	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(TextureFormat format, uint32_t width, uint32_t height, FilterType filterType, bool useMips);
		static Ref<TextureCube> Create(const std::string& path, FilterType filterType, bool useMips);

		//virtual const std::string& GetPath() const = 0;
	};

}