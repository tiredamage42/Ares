#pragma once

#include "AresPCH.h"
namespace Ares
{
	class RenderCommandQueue
	{
	public:
		/*using RenderCommand = std::function<unsigned int(void*)>;
		typedef unsigned int(*RenderCommandFn)(void*);*/
		typedef void(*RenderCommandFn)(void*);

		
		
		RenderCommandQueue();
		~RenderCommandQueue();

		void* Allocate(RenderCommandFn func, uint32_t size);

		
		//void Submit(const RenderCommand& command);
		//void SubmitCommand(RenderCommandFn fn, void* params, unsigned int size);
		void Execute();

	private:
		uint8_t* m_CommandBuffer;
		uint8_t* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
	};
}