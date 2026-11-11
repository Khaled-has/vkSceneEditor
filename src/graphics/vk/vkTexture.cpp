#include "vkTexture.h"
#include "vkLog.h"

namespace GPU
{
#define IS_ALREADY_CREATED(_Crt_, _Msg_)  if (_Crt_) { GPU_LOG_WARN("vkGPU: vkTexture {0} Is already created | destroy it before you try to create it again", _Msg_); return; }

	void vkTexture::SetImage(VkImage mImage, const vkImageCreateInfo& mInfo)
	{
		IS_ALREADY_CREATED(m_ImageCreated, "image");

		m_ImageInfo = mInfo;
		m_Image = mImage;

		m_ImageCreated = true;
	}

	void vkTexture::SetView(VkImageView mView, const vkViewCreateInfo& mInfo)
	{
		IS_ALREADY_CREATED(m_ViewCreated, "view");

		m_ViewInfo = mInfo;
		m_View = mView;

		m_ViewCreated = true;
	}

	void vkTexture::SetSampler(VkSampler mSampler, const vkSamplerCreateInfo& mInfo)
	{
		IS_ALREADY_CREATED(m_SamplerCreated, "sampler");

		m_SamplerInfo = mInfo;
		m_Sampler = mSampler;

		m_SamplerCreated = true;
	}

	void CreateImage(const vkImageCreateInfo& mInfo, vkTexture* pTexture)
	{
		IS_ALREADY_CREATED(pTexture->m_ImageCreated, "image");

		pTexture->m_ImageInfo = mInfo;

		VkImageCreateInfo imageInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = NULL,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = mInfo.mFormat,
			.extent = mInfo.mExtent,
			.mipLevels = 1,
			.arrayLayers = mInfo.mLayerCount,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = mInfo.mUsage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		GPU_CHECK(
			vmaCreateImage(
				mInfo.mAllocator,
				&imageInfo, mInfo.mAllocInfo,
				&pTexture->m_Image, &pTexture->m_Allocation,
				NULL
			),
			"vmaCreateImage"
		);

		pTexture->m_ImageCreated = true;
	}

	void CreateView(const vkViewCreateInfo& mInfo, const vkDevice& mDevice, vkTexture* pTexture)
	{
		IS_ALREADY_CREATED(pTexture->m_ViewCreated, "view");

		if (!pTexture->m_ImageCreated)
		{
			GPU_LOG_ERROR("vkGPU: vkTexture you can't create the view if you don't create the image or set it");
			exit(1);
		}

		pTexture->m_ViewInfo = mInfo;

		// # Create image view
		VkImageViewCreateInfo ViewInfo =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.image = pTexture->m_Image,
			.viewType = mInfo.mViewType,
			.format = pTexture->GetImageInfo().mFormat,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			.subresourceRange = {
				.aspectMask = mInfo.mAspect,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = pTexture->m_ImageInfo.mLayerCount
			}
		};

		GPU_CHECK(vkCreateImageView(
			mDevice.GetDevice(), &ViewInfo, NULL, &pTexture->m_View),
			"vkCreateImageView"
		);

		pTexture->m_ViewCreated = true;
	}

	void CreateSampler(const vkSamplerCreateInfo& mInfo, const vkDevice& mDevice, vkTexture* pTexture)
	{
		IS_ALREADY_CREATED(pTexture->m_SamplerCreated, "sampler");

		pTexture->m_SamplerInfo = mInfo;

		VkSamplerCreateInfo SamplerInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = mInfo.mMaxFilter,
			.minFilter = mInfo.mMinFilter,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = mInfo.mAddressMode,
			.addressModeV = mInfo.mAddressMode,
			.addressModeW = mInfo.mAddressMode,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 1,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			.unnormalizedCoordinates = VK_FALSE,
		};

		GPU_CHECK(vkCreateSampler(
			mDevice.GetDevice(), &SamplerInfo, VK_NULL_HANDLE, &pTexture->m_Sampler),
			"vkCreateSampler"
		);

		pTexture->m_SamplerCreated = true;
	}

	void DestroyImage(vkTexture* pTexture)
	{
		if (!pTexture->IsImageCreated())
		{
			GPU_LOG_WARN("vkGPU: vkTexture you can't destroy image when you don't create it");
			return;
		}

		// # Destroy the image
		vmaDestroyImage(pTexture->m_ImageInfo.mAllocator, pTexture->m_Image, pTexture->m_Allocation);

		// # Reset the image info 
		pTexture->m_ImageInfo = vkImageCreateInfo{};

		pTexture->m_ImageCreated = false;
	}

	void DestroyView(const vkDevice& mDevice, vkTexture* pTexture)
	{
		if (!pTexture->IsViewCreated())
		{
			GPU_LOG_WARN("vkGPU: vkTexture you can't destroy view when you don't create it");
			return;
		}

		// # Destroy the view
		vkDestroyImageView(mDevice.GetDevice(), pTexture->m_View, NULL);
		
		// # Reset the view info
		pTexture->m_ViewInfo = vkViewCreateInfo{};

		pTexture->m_ViewCreated = false;
	}

	void DestroySampler(const vkDevice& mDevice, vkTexture* pTexture)
	{
		if (!pTexture->IsSamplerCreated())
		{
			GPU_LOG_WARN("vkGPU: vkTexture you can't destroy sampler when you don't create it");
			return;
		}

		// # Destroy the sampler
		vkDestroySampler(mDevice.GetDevice(), pTexture->m_Sampler, NULL);

		// # Reset the sampler info
		pTexture->m_SamplerInfo = vkSamplerCreateInfo{};

		pTexture->m_SamplerCreated = false;
	}

	void DestroyAll(const vkDevice& mDevice, vkTexture* pTexture)
	{
		DestroyImage(pTexture);
		DestroyView(mDevice, pTexture);
		DestroySampler(mDevice, pTexture);
	}

	static bool HasStencilComponent(VkFormat Format)
	{
		return ((Format == VK_FORMAT_D32_SFLOAT_S8_UINT)
			|| (Format == VK_FORMAT_D24_UNORM_S8_UINT));
	}

	void ResetImageLayout(vkTexture& mTexture, VkImageLayout mLayout)
	{
		mTexture.m_Layout = mLayout;
	}

	void TextureLayoutTransition(vkTexture& mTexture, const vkCmdBuf& mCmdBuf, VkImageLayout mNewLayout)
	{
		VkImageLayout mOldLayout = mTexture.GetLayout();
		VkFormat Format = mTexture.GetImageInfo().mFormat;

		mTexture.m_Layout = mNewLayout;

		VkImageMemoryBarrier Barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = 0,
			.dstAccessMask = 0,
			.oldLayout = mOldLayout,
			.newLayout = mNewLayout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = mTexture.GetImage(),
			.subresourceRange = VkImageSubresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = mTexture.GetImageInfo().mLayerCount
			}
		};

		VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
		VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

		if (mNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
			(Format == VK_FORMAT_D16_UNORM) ||
			(Format == VK_FORMAT_X8_D24_UNORM_PACK32) ||
			(Format == VK_FORMAT_D32_SFLOAT) ||
			(Format == VK_FORMAT_S8_UINT) ||
			(Format == VK_FORMAT_D16_UNORM_S8_UINT) ||
			(Format == VK_FORMAT_D24_UNORM_S8_UINT))
		{
			Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(Format)) {
				Barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else {
			Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR)
		{
			Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_GENERAL && mNewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_GENERAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
			mNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} /* Convert back from read-only to updateable */
		else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} /* Convert from updateable texture to shader read-only */
		else if (mOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} /* Convert depth texture from undefined state to depth-stencil buffer */
		else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		} /* Wait for render pass to complete */
		else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = 0; // VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = 0;
			/*
					sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			///		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
					destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			*/
			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} /* Convert back from read-only to color attachment */
		else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		} /* Convert from updateable texture to shader read-only */
		else if (mOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} /* Convert back from read-only to depth attachment */
		else if (mOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		} /* Convert from updateable depth texture to shader read-only */
		else if (mOldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_UNDEFINED && mNewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (mOldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && mNewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
			Barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			Barrier.dstAccessMask = 0;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		else {
			GPU_LOG_ERROR(
				"vkGPU: Unknown Barrier case | OldLayout ( {0} ), NewLayout ( {1} )",
				string_VkImageLayout(mOldLayout),
				string_VkImageLayout(mNewLayout)
				);
			exit(1);
		}

		vkCmdPipelineBarrier(mCmdBuf.GetCmdBuf(), sourceStage, destinationStage,
			0, 0, NULL, 0, NULL, 1, &Barrier);
	}

	void CopyBufferToImage(const vkBuffer& mSrcBuf, const vkTexture& mDstTex, VkBufferImageCopy mRegion, const vkCmdBuf& mCmdBuf)
	{
		// # copy command
		vkCmdCopyBufferToImage(
			mCmdBuf.GetCmdBuf(),
			mSrcBuf.GetBuffer(),
			mDstTex.GetImage(), mDstTex.GetLayout(),
			1u, &mRegion
		);
	}
}