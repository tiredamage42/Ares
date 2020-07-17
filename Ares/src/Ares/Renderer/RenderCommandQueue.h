#pragma once


namespace Ares
{
	class RenderCommandQueue
	{
	public:
		RenderCommandQueue();
		~RenderCommandQueue();
		void* Allocate(RenderCommandFn fn, uint32_t size);
		void Execute();

	private:

	};
}