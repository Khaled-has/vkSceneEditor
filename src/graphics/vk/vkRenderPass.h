#ifndef VKRENDERPASS_H
#define VKRENDERPASS_H

#include "vkConfig.h"
#include "vkCmdBuf.h"
#include "vkTexture.h"

namespace GPU
{
	struct vkDynamicInfo
	{
		std::span<const vkTexture> mColorTextures;
		const vkTexture* pDepthTexture;
		std::span<const VkClearColorValue> mClearColors;
		const VkClearDepthStencilValue* pClearDepth;
		VkRect2D mRenderArea;
	};

	void VKGPU_API BeginDynamicRendering(const vkCmdBuf& mCmdBuf, const vkDynamicInfo& mDynamicInfo);
}

#endif