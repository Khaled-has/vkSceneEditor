#include "vkSwapChain.h"
#include "vkSwapChain.h"
#include "vkSwapChain.h"
#include "vkSwapChain.h"
#include "vkLog.h"

namespace GPU
{
	uint32_t ChooseNumImages(const VkSurfaceCapabilitiesKHR& Capabilities, uint32_t mMinCount, uint32_t mMaxCount)
	{
		uint32_t RequestedNumImage = Capabilities.minImageCount + 1;

		uint32_t FinalNumImages = 0;

		if ((Capabilities.maxImageCount > 0) && (RequestedNumImage > Capabilities.maxImageCount))
		{
			FinalNumImages = Capabilities.maxImageCount;
		}
		else {
			FinalNumImages = RequestedNumImage;
		}

		if (FinalNumImages > mMaxCount)
			FinalNumImages = mMaxCount;

		if (FinalNumImages < mMinCount)
		{
			GPU_LOG_WARN("vkGPU: vkSwapChain warning that the swapchain can't create the min image count");
		}

		return FinalNumImages;
	}

	VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> PresentModes)
	{
		for (uint32_t i = 0; i < PresentModes.size(); i++)
		{
			if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return PresentModes[i];
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkSurfaceFormatKHR ChooseSurfaceFormatAndColorSpace(const std::vector<VkSurfaceFormatKHR>& SurfaceFormats, VkSurfaceFormatKHR mSelectedSurfaceForamt)
	{
		for (uint32_t i = 0; i < SurfaceFormats.size(); i++)
		{
			if ((SurfaceFormats[i].format == mSelectedSurfaceForamt.format) &&
				(SurfaceFormats[i].colorSpace == mSelectedSurfaceForamt.colorSpace))
			{
				return SurfaceFormats[i];
			}
		}

		GPU_LOG_WARN(
			"vkGPU: vkSwapChain don't find your VkSurfaceFormatKHR type Format( {0} ) | ColorSpace( {1} )", 
			string_VkFormat(mSelectedSurfaceForamt.format), 
			string_VkColorSpaceKHR(mSelectedSurfaceForamt.colorSpace)
		);

		for (uint32_t i = 0; i < SurfaceFormats.size(); i++)
		{
			if ((SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM) &&
				(SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
				return SurfaceFormats[i];
			}
		}
		GPU_LOG_WARN("No Format & Color Space");
		return SurfaceFormats[0];
	}

	void TransitionSwapImageToDraw(vkSwapChain& mSwapChain, const vkCmdBuf& mCmdBuf, uint32_t mImageIndex)
	{
		//TextureLayoutTransition(mSwapChain.m_SwapTextures[mImageIndex], mCmdBuf, VK_IMAGE_LAYOUT_GENERAL);
		GPU::ResetImageLayout(mSwapChain.m_SwapTextures[mImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkImageMemoryBarrier Barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,

			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = mSwapChain.m_SwapTextures[mImageIndex].GetImage(),
			.subresourceRange = VkImageSubresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkPipelineStageFlags source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags dst = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		vkCmdPipelineBarrier(mCmdBuf.GetCmdBuf(), source, dst, 0, 0, NULL, 0, NULL, 1, &Barrier);
	}

	void TransitionSwapImageToPresent(vkSwapChain& mSwapChain, const vkCmdBuf& mCmdBuf, uint32_t mImageIndex)
	{
		//TextureLayoutTransition(mSwapChain.m_SwapTextures[mImageIndex], mCmdBuf, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		GPU::ResetImageLayout(mSwapChain.m_SwapTextures[mImageIndex], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		VkImageMemoryBarrier Barrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,

			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = 0,

			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,

			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = mSwapChain.m_SwapTextures[mImageIndex].GetImage(),
			.subresourceRange = VkImageSubresourceRange {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		VkPipelineStageFlags source = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkPipelineStageFlags dst = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		vkCmdPipelineBarrier(mCmdBuf.GetCmdBuf(), source, dst, 0, 0, NULL, 0, NULL, 1, &Barrier);
	}

	void CreateSwapChain(const vkSwapChainCreateInfo& mInfo, const vkDevice& mDevice, VkSurfaceKHR mSurface, vkSwapChain* pSwapChain)
	{
		// # Choose num swapchain images
		VkSurfaceCapabilitiesKHR SurfaceCaps;
		GPU_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice.GetPhysDevice().GetPhysDevice(), mSurface, &SurfaceCaps), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
		pSwapChain->m_Extent = VkRect2D{ .offset {.x = 0, .y = 0}, .extent = SurfaceCaps.currentExtent };
		pSwapChain->m_SwapImageCount = ChooseNumImages(SurfaceCaps, mInfo.mMinSwapImageCount, mInfo.mMaxSwapImageCount);

		// # Choose present mode
		const std::vector<VkPresentModeKHR> PresentModes = mDevice.GetPhysDevice().GetInfo().mPresentModes;
		VkPresentModeKHR PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		bool SelecteChosenPrsentMode = false;

		for (auto& m : PresentModes)
		{
			if (mInfo.mPresentMode == m)
			{
				PresentMode = mInfo.mPresentMode;
				SelecteChosenPrsentMode = true;
				break;
			}
		}

		if (!SelecteChosenPrsentMode)
		{
			GPU_LOG_WARN(
				"vkGPU: vkSwapChain the selected present mode {0} is not supported on this device | the current swapchain created with {1}",
				string_VkPresentModeKHR(mInfo.mPresentMode), string_VkPresentModeKHR(PresentMode)
			);
		}
		pSwapChain->m_PresentMode = PresentMode;

		// # Choose swapchain surface format & color space
		pSwapChain->m_SurfaceFormat = ChooseSurfaceFormatAndColorSpace(mDevice.GetPhysDevice().GetInfo().mSurfaceFormats, mInfo.mSurfaceFormat);

		// # Create the swapchain
		VkSwapchainCreateInfoKHR SwapChainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = NULL,
			.flags = 0,
			.surface = mSurface,
			.minImageCount = pSwapChain->m_SwapImageCount,
			.imageFormat = pSwapChain->m_SurfaceFormat.format,
			.imageColorSpace = pSwapChain->m_SurfaceFormat.colorSpace,
			.imageExtent = SurfaceCaps.currentExtent,
			.imageArrayLayers = 1,
			.imageUsage = mInfo.mImageUsage,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = SurfaceCaps.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = PresentMode,
			.clipped = VK_TRUE
		};

		GPU_CHECK(vkCreateSwapchainKHR(mDevice.GetDevice(), &SwapChainCreateInfo, NULL, &pSwapChain->m_Swapchain), "vkCreateSwapchainKHR");

		// # Get the swapchain's images
		uint32_t NumSwapChainImages = 0;
		GPU_CHECK(vkGetSwapchainImagesKHR(mDevice.GetDevice(), pSwapChain->m_Swapchain, &NumSwapChainImages, NULL), "vkGetSwapchainImagesKHR");
		pSwapChain->m_SwapImageCount = NumSwapChainImages;
		GPU_ASSERT(pSwapChain->m_SwapImageCount == NumSwapChainImages, "Num swapchain images != NumSwapChainImages");

		pSwapChain->m_SwapTextures.resize(NumSwapChainImages);

		std::vector<VkImage> Images(NumSwapChainImages);
		pSwapChain->m_SwapTextures.resize(NumSwapChainImages);

		GPU_CHECK(vkGetSwapchainImagesKHR(mDevice.GetDevice(), pSwapChain->m_Swapchain, &NumSwapChainImages, Images.data()), "vkGetSwapchainImagesKHR");

		// # Create swapchain image views
		for (uint32_t i = 0; i < NumSwapChainImages; i++)
		{
			vkImageCreateInfo imageInfo = {
				.mFormat = pSwapChain->m_SurfaceFormat.format,
				.mExtent = VkExtent3D { .width = SurfaceCaps.currentExtent.width, .height = SurfaceCaps.currentExtent.height, .depth = 1u },
				.mUsage = mInfo.mImageUsage,
				.mLayerCount = 1
			};

			vkViewCreateInfo viewInfo = {
				.mViewType = VK_IMAGE_VIEW_TYPE_2D,
				.mAspect = VK_IMAGE_ASPECT_COLOR_BIT
			};

			vkSamplerCreateInfo samplerInfo = {
				.mMinFilter = VK_FILTER_LINEAR,
				.mMaxFilter = VK_FILTER_LINEAR,
				.mAddressMode = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
			};

			vkTexture& CurrentTexture = pSwapChain->m_SwapTextures[i];

			// # Set the image from the swapchain
			CurrentTexture.SetImage(Images[i], imageInfo);

			// # Create the view
			GPU::CreateView(viewInfo, mDevice, &CurrentTexture);
			// # Create the sampler
			GPU::CreateSampler(samplerInfo, mDevice, &CurrentTexture);
		}
	}

	void GPU::AcquireNextImage(const vkDevice& mDevice, const vkSwapChain& mSwapChain, uint64_t mTimeOut, const vkSemaphore* pSemaphore, const vkFence* pFence, uint32_t* pImageIndex, vkSwapChainResult* pSwapResult)
	{
		// # Acquire the next image
		VkResult Res = vkAcquireNextImageKHR(
			mDevice.GetDevice(), mSwapChain.GetSwapchain(), 
			mTimeOut,
			pSemaphore ? pSemaphore->GetSemaphore() : VK_NULL_HANDLE,
			pFence	? pFence->GetFence() : VK_NULL_HANDLE,
			pImageIndex
			);

		// # Check if the swapchain want to recreate or stop present
		if (pSwapResult)
		{

		}
	}
}