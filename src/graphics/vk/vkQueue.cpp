#include "vkQueue.h"
#include "vkLog.h"

namespace GPU
{
	void PushSemaphores(std::span<const vkSemaphore> mSemaSrc, std::vector<VkSemaphore>& mSemaDst)
	{
		for (const auto& Sema : mSemaSrc)
		{
			mSemaDst.push_back(Sema.GetSemaphore());
		}
	}

	void GetDeviceQueue(const vkDevice& mDevice, VkQueueFlagBits mQType, vkQueue* pQueue)
	{
		int32_t QFamilyIndex = -1;
		// # Graphics
		if ((mQType & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) 
			QFamilyIndex = mDevice.GetDeviceQueueInfo().mGraphics;
		// # Compute
		else if ((mQType & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) 
			QFamilyIndex = mDevice.GetDeviceQueueInfo().mCompute;
		// # Transfer
		else if ((mQType & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) 
			QFamilyIndex = mDevice.GetDeviceQueueInfo().mTransfer;
		
		pQueue->m_QFamilyIndex = QFamilyIndex;
		
		vkGetDeviceQueue(mDevice.GetDevice(), QFamilyIndex, 0, &pQueue->m_Queue);
	}

	void SubmitQueue(const vkQueue& mQueue, const vkSubmitInfo& mSubmitInfo, const vkFence* pFence)
	{
		SubmitQueue(mQueue, std::span{ &mSubmitInfo, 1 }, pFence);
	}

	void SubmitQueue(const vkQueue& mQueue, std::span<const vkSubmitInfo> mSubmitInfo, const vkFence* pFence)
	{
		// # Get the cmds of each submit info
		std::vector<std::vector<VkCommandBuffer>> SubmitCmds(mSubmitInfo.size());

		for (uint32_t s = 0; s < mSubmitInfo.size(); s++)
		{
			// # For one cmd
			if ((mSubmitInfo[s].pCmd) && mSubmitInfo[s].mCmds.size() == 0)
			{
				// # Add the cmd
				SubmitCmds[s].push_back(mSubmitInfo[s].pCmd->GetCmdBuf());
			}
			// # For multiple cmds
			else if (mSubmitInfo[s].mCmds.size() > 0 && !(mSubmitInfo[s].pCmd))
			{
				for (uint32_t c = 0; c < mSubmitInfo[s].mCmds.size(); c++)
				{
					SubmitCmds[s].push_back(mSubmitInfo[s].mCmds[c].GetCmdBuf());
				}
			}
			else if (!(mSubmitInfo[s].pCmd) && mSubmitInfo[s].mCmds.size() == 0)
			{
				GPU_LOG_ERROR("vkGPU: SubmitQueue() function error: You can't keep the vkSubmitInfo.mCmds & .pCmd empty");
				exit(1);
			}
			// # Error if the user put cmds on the ( mCmds & mCmd )
			else
			{
				GPU_LOG_ERROR("vkGPU: SubmitQueue() function error: You can't put cmds twice on the ( vkSubmitInfo.mCmds & vkSubmitInfo.pCmd )");
				exit(1);
			}
		}

		// # Create the submit structures
		std::vector<VkSubmitInfo> Submits;

		std::vector<std::vector<VkSemaphore>> WaitSemaphores(mSubmitInfo.size());
		std::vector<std::vector<VkPipelineStageFlags>> WaitDstStageFlags(mSubmitInfo.size());

		std::vector<std::vector<VkSemaphore>> SignalSemaphores(mSubmitInfo.size());

		uint32_t SubIndex = 0;
		for (const auto& Sub : mSubmitInfo)
		{
			// # Check the count of wait semaphores & pipeline stage flags
			if (
				(Sub.pWaitSemaphore) && Sub.mWaitSemaphores.size() == 0 &&
				(Sub.mWaitDstStageMask) && Sub.mWaitDstStageMaskes.size() == 0
				)
			{
				WaitSemaphores[SubIndex].push_back(Sub.pWaitSemaphore->GetSemaphore());
				WaitDstStageFlags[SubIndex].push_back(Sub.mWaitDstStageMask);
			}
			else if (
				!(Sub.pWaitSemaphore) && Sub.mWaitSemaphores.size() > 0 &&
				!(Sub.mWaitDstStageMask) && Sub.mWaitDstStageMaskes.size() > 0
				)
			{
				PushSemaphores(Sub.mWaitSemaphores, WaitSemaphores[SubIndex]);

				for (const auto& F : Sub.mWaitDstStageMaskes) 
					WaitDstStageFlags[SubIndex].push_back(F);
			}
			else if (
				!(Sub.pWaitSemaphore) && Sub.mWaitSemaphores.size() == 0 &&
				!(Sub.mWaitDstStageMask) && Sub.mWaitDstStageMaskes.size() == 0
				)
			{
				// # Don't do anything -> If the user doesn't want the wait semaphores that his choice
			}
			else
			{
				GPU_LOG_ERROR(
					R"(
					vkGPU: SubmitQueue() error: 
					You should to minmize the number of ( mWaitSemaphores & mWaitDstStageMaskes ) & 
					Don't use the single pWaitSemaphore with the multiple mWaitSemaphores
					)"
				);

				exit(1);
			}

			// # Check the count of signal semaphores
			

			if ((Sub.pSignalSemaphore) && Sub.mSignalSemaphores.size() == 0)
			{
				SignalSemaphores[SubIndex].push_back(Sub.pSignalSemaphore->GetSemaphore());
			}
			else if (!(Sub.pSignalSemaphore) && Sub.mSignalSemaphores.size() > 0)
			{
				PushSemaphores(Sub.mSignalSemaphores, SignalSemaphores[SubIndex]);
			}
			else if (!(Sub.pSignalSemaphore) && Sub.mSignalSemaphores.size() == 0)
			{
				// # Don't do anything -> If the user doesn't want the signal semaphores that his choice
			}
			else
			{
				GPU_LOG_ERROR("vkGPU: SubmitQueue() function error: You can't put cmds twice on the ( vkSubmitInfo.mSignalSemaphores & vkSubmitInfo.pSignalSemaphore )");
				exit(1);
			}

			// # Create the submit info
			Submits.push_back(
				VkSubmitInfo{
					.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
					.pNext = NULL,

					.waitSemaphoreCount = (uint32_t)WaitSemaphores[SubIndex].size(),
					.pWaitSemaphores = WaitSemaphores[SubIndex].data(),
					.pWaitDstStageMask = WaitDstStageFlags[SubIndex].data(),

					.commandBufferCount = (uint32_t)SubmitCmds[SubIndex].size(),
					.pCommandBuffers = SubmitCmds[SubIndex].data(),

					.signalSemaphoreCount = (uint32_t)SignalSemaphores[SubIndex].size(),
					.pSignalSemaphores = SignalSemaphores[SubIndex].data(),
				}
			);

			SubIndex++;
		}

		GPU_CHECK(
			vkQueueSubmit(mQueue.GetQueue(), (uint32_t)Submits.size(), Submits.data(), pFence ? pFence->GetFence() : VK_NULL_HANDLE), 
			"vkQueueSubmit"
		);
	}

	void SubmitQueue2(const vkQueue& mQueue, const vkSubmitInfo2& mSubmitInfo, const vkFence* pFence)
	{
		SubmitQueue2(mQueue, std::span{ &mSubmitInfo, 1 }, pFence);
	}

	void SubmitQueue2(const vkQueue& mQueue, std::span<const vkSubmitInfo2> mSubmitInfos, const vkFence* pFence)
	{

	}

	void WaitIdle(const vkQueue& mQueue)
	{
		vkQueueWaitIdle(mQueue.GetQueue());
	}

}