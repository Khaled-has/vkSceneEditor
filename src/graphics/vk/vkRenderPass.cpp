#include "vkRenderPass.h"
#include "vkLog.h"

namespace GPU
{
	void BeginDynamicRendering(const vkCmdBuf& mCmdBuf, const vkDynamicInfo& mDynamicInfo)
	{
		std::vector <VkRenderingAttachmentInfoKHR> ColorAttachments;
		for (uint32_t i = 0; i < mDynamicInfo.mColorTextures.size(); i++)
		{
			VkRenderingAttachmentInfoKHR ColorAttach = {
				.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
				.pNext = NULL,
				.imageView = mDynamicInfo.mColorTextures[i].GetView(),
				.imageLayout = mDynamicInfo.mColorTextures[i].GetLayout(),
				.resolveMode = VK_RESOLVE_MODE_NONE,
				.resolveImageView = VK_NULL_HANDLE,
				.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.loadOp = &mDynamicInfo.mClearColors[i] ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE
			};

			if (&mDynamicInfo.mClearColors[i])
			{
				ColorAttach.clearValue.color = mDynamicInfo.mClearColors[i];
			}

			ColorAttachments.push_back(ColorAttach);
		}

		VkRenderingAttachmentInfo DepthAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = NULL,
			.imageView = mDynamicInfo.pDepthTexture ? mDynamicInfo.pDepthTexture->GetView() : VK_NULL_HANDLE,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = mDynamicInfo.pClearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE
		};

		if (mDynamicInfo.pClearDepth)
		{
			DepthAttachment.clearValue.depthStencil = *mDynamicInfo.pClearDepth;
		}

		VkRenderingInfoKHR RenderingInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
			.renderArea = mDynamicInfo.mRenderArea,
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = (uint32_t)ColorAttachments.size(),
			.pColorAttachments = ColorAttachments.data(),
			.pDepthAttachment = mDynamicInfo.pDepthTexture ? &DepthAttachment : VK_NULL_HANDLE,
		};

		vkCmdBeginRendering(mCmdBuf.GetCmdBuf(), &RenderingInfo);
	}
}