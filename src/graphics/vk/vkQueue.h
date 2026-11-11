#pragma once

#include "vkSync.h"

namespace GPU
{
	// # Submit info 
	struct vkSubmitInfo
	{
		const vkSemaphore* pWaitSemaphore;
		VkPipelineStageFlags mWaitDstStageMask;
		std::span<const vkSemaphore> mWaitSemaphores;
		std::span<const VkPipelineStageFlags> mWaitDstStageMaskes;

		const vkCmdBuf* pCmd;
		std::span<const vkCmdBuf> mCmds;

		const vkSemaphore* pSignalSemaphore;
		std::span<const vkSemaphore> mSignalSemaphores;
	};

	// # Submit info 2
	struct vkSubmitInfo2
	{
	};

	class VKGPU_API vkQueue
	{
		friend void GetDeviceQueue(const vkDevice& mDevice, VkQueueFlagBits mQType, vkQueue* pQueue);
		
		friend void SubmitQueue(const vkQueue& mQueue, const vkSubmitInfo& mSubmitInfo, const vkFence* pFence);
		friend void SubmitQueue(const vkQueue& mQueue, std::span<const vkSubmitInfo> mSubmitInfos, const vkFence* pFence);

		friend void SubmitQueue2(const vkQueue& mQueue, const vkSubmitInfo2& mSubmitInfo, const vkFence* pFence);
		friend void SubmitQueue2(const vkQueue& mQueue, std::span<const vkSubmitInfo2> mSubmitInfos, const vkFence* pFence);
	public:
		vkQueue() {}

		inline const VkQueue& GetQueue() const { return m_Queue; }

		inline uint32_t GetQueueIndex() const { return m_QueueIndex; }
		inline uint32_t GetQFamilyIndex() const { return m_QFamilyIndex; }

	private:
		VkQueue m_Queue = VK_NULL_HANDLE;
		uint32_t m_QueueIndex = 0;
		uint32_t m_QFamilyIndex = 0;
	};

	VKGPU_API void GetDeviceQueue(const vkDevice& mDevice, VkQueueFlagBits mQType, vkQueue* pQueue);

	VKGPU_API void SubmitQueue(const vkQueue& mQueue, const vkSubmitInfo& mSubmitInfo, const vkFence* pFence);
	VKGPU_API void SubmitQueue(const vkQueue& mQueue, std::span<const vkSubmitInfo> mSubmitInfos, const vkFence* pFence);

	VKGPU_API void SubmitQueue2(const vkQueue& mQueue, const vkSubmitInfo2& mSubmitInfo, const vkFence* pFence);
	VKGPU_API void SubmitQueue2(const vkQueue& mQueue, std::span<const vkSubmitInfo2> mSubmitInfos, const vkFence* pFence);

	VKGPU_API void WaitIdle(const vkQueue& mQueue);
}