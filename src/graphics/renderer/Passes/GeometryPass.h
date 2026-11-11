#ifndef GEOMETRYPASS_H
#define GEOMETRYPASS_H

#include "RenderGraph.h"

class GeometryPass : public RenderPass
{
public:
	GeometryPass() : RenderPass(GPU_RENDER_PASS_TYPE_GRAPHICS, "GeometryPass") 
	{}

	virtual void Destroy() override;

	virtual void Setup(PassSetup& mSetup) override;
	virtual void Init(const RenderResources& mResources, PassInit& mInit) override;

	virtual void Execute(const GPU::vkCmdBuf& mCmdBuf, uint32_t mSwapIndex) override;

private:
	GPU::vkPipeline m_Pipeline;

	VkDescriptorSetLayout m_DescLayout;
	GPU::vkDescriptorSet m_DescSet;
};

#endif