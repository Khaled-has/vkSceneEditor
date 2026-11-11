#include "vkRenderTarget.h"
#include "vkRenderer.h"

#include <imgui_impl_vulkan.h>

void vkRenderTarget::SetupPipeline(gfx::RenderResolution mRenderRes)
{
	const RenderResources& RenderRes = vkRenderer::GetRenderResources();

	m_pRenderGraph = std::make_unique<RenderGraph>(RenderRes.GetDevice(), RenderRes.GetAllocator());

	if (m_PipeType == gfx::GFX_PIPELINE_TYPE_RASTERZATION)
	{
		m_pRenderGraph->AddRenderPass<GeometryPass>();
	}

	m_pRenderGraph->Setup(RenderRes, vkRenderer::GetGraphicsQueue());

	const GPU::vkTexture* OutImage;
	if (m_PipeType == gfx::GFX_PIPELINE_TYPE_RASTERZATION)
	{
		OutImage = m_pRenderGraph->GetImage("GBuffer.BaseColor", VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	else if (m_PipeType == gfx::GFX_PIPELINE_TYPE_PATH_TRACHING)
	{

	}

	m_RenderImage = (ImTextureID)ImGui_ImplVulkan_AddTexture(
		OutImage->GetSampler(), OutImage->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	);
}

void vkRenderTarget::ResetRenderResolution(gfx::RenderResolution)
{
}

const GPU::vkCmdBuf& vkRenderTarget::Execute(uint32_t mSwapIndex)
{
	return m_pRenderGraph->Execute(mSwapIndex);
}
