#include "vkPipeline.h"
#include "vkLog.h"

namespace GPU
{
	void CreateGraphicsPipeline(const vkDevice& mDevice, const vkPipeLayoutCreateInfo& mLayoutInfo, const vkGraphicsPipelineCreateInfo& mPipeInfo, vkPipeline* pPipeline)
	{
		pPipeline->m_BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		// # Create pipeline layout
		VkPipelineLayoutCreateInfo LayoutInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = NULL,
			.setLayoutCount = (uint32_t)mLayoutInfo.mDescSetLayouts.size(),
			.pSetLayouts = mLayoutInfo.mDescSetLayouts.data()
		};

		GPU_CHECK(vkCreatePipelineLayout(mDevice.GetDevice(), &LayoutInfo, NULL, &pPipeline->m_PipeLayout), "vkCreatePipelineLayout");

		VkPipelineVertexInputStateCreateInfo VertexInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
		};

		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				.primitiveRestartEnable = VK_FALSE,
		};

		VkPipelineRasterizationStateCreateInfo RastInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.lineWidth = 1.0f
		};

		VkPipelineMultisampleStateCreateInfo MultiSampleInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.0f
		};

		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE,
			.front = {},
			.back = {},
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f
		};

		VkPipelineColorBlendAttachmentState BlendAttachState = {
			.blendEnable = VK_FALSE,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
		};

		VkPipelineColorBlendStateCreateInfo BlendInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &BlendAttachState
		};

		VkGraphicsPipelineCreateInfo GraphicsInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = mPipeInfo.pNext,

			.stageCount = (uint32_t)mPipeInfo.mShaderStages.size(),
			.pStages = mPipeInfo.mShaderStages.data(),

			.pVertexInputState = mPipeInfo.pVertexInput ? mPipeInfo.pVertexInput : &VertexInfo,
			.pInputAssemblyState = mPipeInfo.pInputAssembly ? mPipeInfo.pInputAssembly : &InputAssemblyInfo,
			.pViewportState = mPipeInfo.pViewport,
			.pRasterizationState = mPipeInfo.pRasterization ? mPipeInfo.pRasterization : &RastInfo,
			.pMultisampleState = mPipeInfo.pMultisample ? mPipeInfo.pMultisample : &MultiSampleInfo,
			.pDepthStencilState = mPipeInfo.pDepthStencil ? mPipeInfo.pDepthStencil : &DepthStencilInfo,
			.pColorBlendState = mPipeInfo.pColorBlend ? mPipeInfo.pColorBlend : &BlendInfo,
			.pDynamicState = mPipeInfo.pDynamicState ? mPipeInfo.pDynamicState : NULL,

			.layout = pPipeline->GetPipeLayout(),
			//.renderPass = mPipeInfo.pColorBlend->GetRenderPass(),
			.renderPass = NULL,
			.subpass = mPipeInfo.mSubpass,
			.basePipelineHandle = mPipeInfo.mBasePipeHandle,
			.basePipelineIndex = mPipeInfo.mBasePipeIndex
		};

		GPU_CHECK(vkCreateGraphicsPipelines(mDevice.GetDevice(), VK_NULL_HANDLE, 1, &GraphicsInfo, NULL, &pPipeline->m_Pipeline), "vkCreateGraphicsPipelines");
	}

	void BindPipeline(const vkCmdBuf& mCmdBuf, const vkPipeline& mPipeline)
	{
		vkCmdBindPipeline(mCmdBuf.GetCmdBuf(), mPipeline.GetBindPoint(), mPipeline.GetPipeline());
	}
}