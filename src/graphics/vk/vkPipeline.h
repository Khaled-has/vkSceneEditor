#ifndef VKPIPELINE_H
#define VKPIPELINE_H

#include "vkCmdBuf.h"

namespace GPU
{
	struct vkPipeLayoutCreateInfo
	{
		std::span<const VkDescriptorSetLayout> mDescSetLayouts;
	};

	struct vkGraphicsPipelineCreateInfo
	{
		const void* pNext;
		std::span<const VkPipelineShaderStageCreateInfo> mShaderStages;

		const VkPipelineVertexInputStateCreateInfo*		pVertexInput;
		const VkPipelineInputAssemblyStateCreateInfo*	pInputAssembly;
		const VkPipelineViewportStateCreateInfo*		pViewport;
		const VkPipelineRasterizationStateCreateInfo*	pRasterization;
		const VkPipelineMultisampleStateCreateInfo*		pMultisample;
		const VkPipelineDepthStencilStateCreateInfo*	pDepthStencil;
		const VkPipelineColorBlendStateCreateInfo*		pColorBlend;
		const VkPipelineDynamicStateCreateInfo*			pDynamicState;

		// const vkRenderPass* pRenderPass;
		uint32_t mSubpass;
		VkPipeline mBasePipeHandle = VK_NULL_HANDLE;
		int32_t mBasePipeIndex = -1;
	};

	class VKGPU_API vkPipeline
	{
		friend void CreateGraphicsPipeline(const vkDevice& mDevice, const vkPipeLayoutCreateInfo& mLayoutInfo, const vkGraphicsPipelineCreateInfo& mPipeInfo, vkPipeline* pPipeline);
	public:
		vkPipeline() {}

		inline const VkPipeline& GetPipeline()				const { return m_Pipeline; }
		inline const VkPipelineLayout& GetPipeLayout()		const { return m_PipeLayout; }
		inline const VkPipelineBindPoint& GetBindPoint()	const { return m_BindPoint; }

	private:
		VkPipeline m_Pipeline			= VK_NULL_HANDLE;
		VkPipelineLayout m_PipeLayout	= VK_NULL_HANDLE;
		VkPipelineBindPoint m_BindPoint;
	};

	VKGPU_API void CreateGraphicsPipeline(const vkDevice& mDevice, const vkPipeLayoutCreateInfo& mLayoutInfo, const vkGraphicsPipelineCreateInfo& mPipeInfo, vkPipeline* pPipeline);

	VKGPU_API void BindPipeline(const vkCmdBuf& mCmdBuf, const vkPipeline& mPipeline);
}

#endif