#pragma once

#include "AresPCH.h"
namespace Ares
{
	class RenderCommandQueue
	{
	public:
		RenderCommandQueue();
		~RenderCommandQueue();

		using RenderCommand = std::function<unsigned int(void*)>;
		typedef unsigned int(*RenderCommandFn)(void*);

		
		//void Submit(const RenderCommand& command);
		void SubmitCommand(RenderCommandFn fn, void* params, unsigned int size);
		void Execute();

	private:
		unsigned char* m_CommandBuffer;
		unsigned char* m_CommandBufferPtr;
		unsigned int m_RenderCommandCount = 0;
	};
}