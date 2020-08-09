#pragma once

#include <glm/glm.hpp>

#include "Ares/Core/Core.h"

namespace Ares
{
	enum class FramebufferFormat
	{
		None = 0,
		RGBA8 = 1,
		RGBA16F = 2
	};

	struct FrameBufferSpecs
	{
		uint32_t Width = 0, Height = 0;
		glm::vec4 ClearColor;
		FramebufferFormat Format;

		uint32_t Samples = 1;

		// true = render to screen
		bool SwapChainTarget = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;
		virtual const FrameBufferSpecs& GetSpecs() const = 0;
		
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void BindAsTexture(uint32_t slot = 0) const = 0;

		virtual bool Resize(uint32_t width, uint32_t height, bool force=false) = 0;
		
		virtual uint32_t GetRendererID() const = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;
		
		static Ref<FrameBuffer> Create(const FrameBufferSpecs& specs);
	};


	class FramebufferPool final
	{
	public:
		FramebufferPool(uint32_t maxFBs = 32);
		~FramebufferPool();

		std::weak_ptr<FrameBuffer> AllocateBuffer();
		void Add(const Ref<FrameBuffer>& framebuffer);

		std::vector<Ref<FrameBuffer>>& GetAll() { return m_Pool; }
		const std::vector<Ref<FrameBuffer>>& GetAll() const { return m_Pool; }

		inline static FramebufferPool* GetGlobal() { return s_Instance; }
	private:
		std::vector<Ref<FrameBuffer>> m_Pool;

		static FramebufferPool* s_Instance;
	};
}