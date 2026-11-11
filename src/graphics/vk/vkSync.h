#ifndef VKSYNC_H
#define VKSYNC_H

#include "vkDevice.h"
#include "vkCmdBuf.h"

namespace GPU
{
	class VKGPU_API vkSemaphore
	{
		friend void CreateSyncSemaphore(const vkDevice& mDevice, vkSemaphore* pSemaphore, VkSemaphoreCreateFlags mFlags);
		friend void DestroySemaphore(vkSemaphore* pSemaphore);
	public:
		vkSemaphore() {}

		inline const VkSemaphore& GetSemaphore() const { return m_Semaphore; }

	private:
		VkSemaphore m_Semaphore = VK_NULL_HANDLE;
		const vkDevice* m_pDevice = nullptr;
	};

	class VKGPU_API vkFence
	{
		friend void CreateFence(const vkDevice& mDevice, vkFence* pFence, VkFenceCreateFlags mFlags);
		friend void DestroyFence(vkFence* pFence);
	public:
		vkFence() {}

		inline const VkFence& GetFence() const { return m_Fence; }

	private:
		VkFence m_Fence = VK_NULL_HANDLE;
		const vkDevice* m_pDevice = nullptr;
	};

	VKGPU_API void CreateSyncSemaphore(const vkDevice& mDevice, vkSemaphore* pSemaphore, VkSemaphoreCreateFlags mFlags = 0);
	VKGPU_API void DestroySemaphore(vkSemaphore* pSemaphore);

	VKGPU_API void CreateFence(const vkDevice& mDevice, vkFence* pFence, VkFenceCreateFlags mFlags = 0);
	VKGPU_API void DestroyFence(vkFence* pFence);
}

#endif