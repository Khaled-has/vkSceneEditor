#include "vkSync.h"
#include "vkLog.h"

namespace GPU
{
	void CreateSyncSemaphore(const vkDevice& mDevice, vkSemaphore* pSemaphore, VkSemaphoreCreateFlags mFlags /*= 0*/)
	{
		pSemaphore->m_pDevice = &mDevice;

		VkSemaphoreCreateInfo Info = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = mFlags
		};

		GPU_CHECK(vkCreateSemaphore(mDevice.GetDevice(), &Info, NULL, &pSemaphore->m_Semaphore), "vkCreateSemaphore");
	}

	void DestroySemaphore(vkSemaphore* pSemaphore)
	{
		vkDestroySemaphore(pSemaphore->m_pDevice->GetDevice(), pSemaphore->m_Semaphore, NULL);
	}

	void CreateFence(const vkDevice& mDevice, vkFence* pFence, VkFenceCreateFlags mFlags /*= 0*/)
	{
		pFence->m_pDevice = &mDevice;

		VkFenceCreateInfo Info = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = mFlags
		};

		GPU_CHECK(vkCreateFence(mDevice.GetDevice(), &Info, NULL, &pFence->m_Fence), "vkCreateFence");
	}

	void DestroyFence(vkFence* pFence)
	{
		vkDestroyFence(pFence->m_pDevice->GetDevice(), pFence->m_Fence, NULL);
	}
}