#include "GeometryPass.h"

void GeometryPass::Setup(PassSetup& mSetup)
{
	ImageInfo BaseColorInfo = {
		.mUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.mAspect = VK_IMAGE_ASPECT_COLOR_BIT,
		.mFormat = VK_FORMAT_B8G8R8A8_UNORM,
		.mExtent  {1440, 720}
	};

	mSetup.AddImage("GBuffer.BaseColor", BaseColorInfo);

	mSetup.WriteImage("GBuffer.BaseColor");
}

void GeometryPass::Destroy()
{

}

void GeometryPass::Init(const RenderResources& mResources, PassInit& mInit)
{
	// # Create pipeline
	VkPipelineShaderStageCreateInfo ShaderStages[] = {
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = mResources.GetShaderResource("geometry.vert").GetShaderModule(),
			.pName = "main"
		},
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = mResources.GetShaderResource("geometry.frag").GetShaderModule(),
			.pName = "main"
		}
	};

	VkPipelineRenderingCreateInfo RenderingInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &mInit.GetImage_Write("GBuffer.BaseColor").GetImageInfo().mFormat
	};

	VkViewport ViewportInfo = {
		.x = 0, .y = 0,
		.width = 1440.0f, .height = 720.0f,
		.minDepth = 0.0, .maxDepth = 1.0
	};

	VkRect2D Scissor = {
		.offset = {0, 0}, .extent{ 1440, 720 }
	};

	VkPipelineViewportStateCreateInfo ViewportStateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &ViewportInfo,
		.scissorCount = 1,
		.pScissors = &Scissor
	};

	VkDynamicState DynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo DynamicStateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = uint32_t(sizeof(DynamicStates) / sizeof(VkDynamicState)),
		.pDynamicStates = DynamicStates
	};

	m_DescLayout = mResources.GetDescriptorResource("Descriptor_0").mDescLayout;
	m_DescSet = mResources.GetDescriptorResource("Descriptor_0").mDescSet;

	GPU::vkPipeLayoutCreateInfo LayoutInfo = {
		.mDescSetLayouts = std::span{ &m_DescLayout, 1 }
	};

	GPU::vkGraphicsPipelineCreateInfo GraphicsInfo = {
		.pNext = &RenderingInfo,
		.mShaderStages = ShaderStages,
		.pViewport = &ViewportStateInfo,
		.pDynamicState = &DynamicStateInfo
	};

	GPU::CreateGraphicsPipeline(mResources.GetDevice(), LayoutInfo, GraphicsInfo, &m_Pipeline);
}

void GeometryPass::Execute(const GPU::vkCmdBuf& mCmdBuf, uint32_t mSwapIndex)
{
	GPU::BindPipeline(mCmdBuf, m_Pipeline);

	VkViewport Viewport = { 0, 0, 1440, 720, 0.0f, 1.0f };
	VkRect2D Sicssor = { {0, 0}, {1440, 720} };

	vkCmdSetViewport(mCmdBuf.GetCmdBuf(), 0, 1, &Viewport);
	vkCmdSetScissor(mCmdBuf.GetCmdBuf(), 0, 1, &Sicssor);

	vkCmdBindDescriptorSets(
		mCmdBuf.GetCmdBuf(), m_Pipeline.GetBindPoint(),
		m_Pipeline.GetPipeLayout(), 0, 1,
		&m_DescSet.GetDescSet(), 0, NULL
	);

	vkCmdDraw(mCmdBuf.GetCmdBuf(), 3, 1, 0, 0);
}
