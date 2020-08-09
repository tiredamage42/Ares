#pragma once

#include "FrameBuffer.h"

namespace Ares
{

	struct RenderPassSpecs
	{
		Ref<FrameBuffer> TargetFrameBuffer;
	};
	class RenderPass
	{
	public:
		RenderPass(const RenderPassSpecs& specs)
			: m_Specs(specs)
		{

		}
		~RenderPass() = default;

		inline RenderPassSpecs& GetSpecs() { return m_Specs; }
		inline const RenderPassSpecs& GetSpecs() const { return m_Specs; }

	private:
		RenderPassSpecs m_Specs;

	};

	
}