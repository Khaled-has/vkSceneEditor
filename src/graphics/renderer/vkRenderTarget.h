#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "Renderer.h"
#include "Passes/RenderGraph.h"

class vkRenderTarget : public gfx::RenderTarget
{
public:
	vkRenderTarget(Camera* pCamera, gfx::PipelineType mPipeType)
		: gfx::RenderTarget(pCamera, m_PipeType)
	{}

	virtual void SetupPipeline(gfx::RenderResolution mRenderRes) override;
	virtual void ResetRenderResolution(gfx::RenderResolution) override;

	const GPU::vkCmdBuf& Execute(uint32_t mSwapIndex);

private:
	std::unique_ptr<RenderGraph> m_pRenderGraph;
};

#endif