#pragma once

#include "AresPCH.h"
namespace Ares
{
	class RenderCommandQueue
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		RenderCommandQueue();
		~RenderCommandQueue();

		void* Allocate(RenderCommandFn func, uint32_t size, const std::string& commandName);
		void Execute();

	private:
		bool m_Deleted = false;
		uint8_t* m_CommandBuffer;
		uint8_t* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
		std::vector<std::string> m_CommandNames;

		friend class Renderer;
	};
}