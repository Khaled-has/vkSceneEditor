#ifndef VKSWAPCHAIN_H
#define VKSWAPCHAIN_H

#include "vkTexture.h"
#include "vkSync.h"

namespace GPU
{
	struct vkSwapChainCreateInfo
	{
		uint32_t mMinSwapImageCount;
		uint32_t mMaxSwapImageCount;
		
		VkSurfaceFormatKHR mSurfaceFormat;
		VkPresentModeKHR mPresentMode;
		VkImageUsageFlags mImageUsage;
	};

	struct vkSwapChainResult
	{
		bool mReCreateSwapChain;
		bool mStopPresentSwapChain;
	};

	class VKGPU_API vkSwapChain
	{
		friend void CreateSwapChain(const vkSwapChainCreateInfo& mInfo, const vkDevice& mDevice, VkSurfaceKHR mSurface, vkSwapChain* pSwapChain);

		friend void TransitionSwapImageToDraw(vkSwapChain& mSwapChain, const vkCmdBuf& mCmdBuf, uint32_t mImageIndex);
		friend void TransitionSwapImageToPresent(vkSwapChain& mSwapChain, const vkCmdBuf& mCmdBuf, uint32_t mImageIndex);
	public:
		vkSwapChain() {}

		inline const VkSwapchainKHR& GetSwapchain()				const { return m_Swapchain; }

		inline uint32_t GetSwapImageCount()						const { return m_SwapImageCount; }
		inline VkRect2D GetSwapExtent()							const { return m_Extent; }
		inline VkSurfaceFormatKHR GetSurfaceFormat()			const { return m_SurfaceFormat; }
		inline VkPresentModeKHR GetPresentMode()				const { return m_PresentMode; }

		inline const std::vector<vkTexture>& GetSwapTextures()	const { return m_SwapTextures; }

	private:
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		std::vector<vkTexture> m_SwapTextures;

		VkRect2D m_Extent;
		uint32_t m_SwapImageCount = 0;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
	};

	VKGPU_API void TransitionSwapImageToDraw(vkSwapChain& mSwapChain, const vkCmdBuf& mCmdBuf, uint32_t mImageIndex);
	VKGPU_API void TransitionSwapImageToPresent(vkSwapChain& mSwapChain, const vkCmdBuf& mCmdBuf, uint32_t mImageIndex);

	VKGPU_API void CreateSwapChain(const vkSwapChainCreateInfo& mInfo, const vkDevice& mDevice, VkSurfaceKHR mSurface, vkSwapChain* pSwapChain);

	VKGPU_API void AcquireNextImage(const vkDevice& mDevice, const vkSwapChain& mSwapChain, uint64_t mTimeOut, const vkSemaphore* pSemaphore, const vkFence* pFence, uint32_t* pImageIndex, vkSwapChainResult* pSwapResult);
}

#endif