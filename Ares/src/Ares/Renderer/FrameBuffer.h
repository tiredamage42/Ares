#pragma once

#include "Ares/Core/Core.h"

namespace Ares
{

	struct FrameBufferSpecs
	{
		uint32_t Width = 0, Height = 0;
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
		virtual void Resize(uint32_t width, uint32_t height, bool force=false) = 0;
		virtual uint32_t GetColorAttachmentRendererID() const = 0;
		static Ref<FrameBuffer> Create(const FrameBufferSpecs& specs);
	};
}