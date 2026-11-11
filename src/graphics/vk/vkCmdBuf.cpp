#include "vkCmdBuf.h"
#include "vkLog.h"

namespace GPU
{
	void CreateCmdPool(const vkCmdPoolCreateInfo& mInfo, const vkDevice& mDevice, vkCmdPool* pCmdPool)
	{
		pCmdPool->m_pDevice = &mDevice;

		VkCommandPoolCreateInfo poolInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = mInfo.mFlags,
			.queueFamilyIndex = mInfo.mQueueFamilyIndex
		};

		GPU_CHECK(
			vkCreateCommandPool(mDevice.GetDevice(), &poolInfo, NULL, &pCmdPool->m_CmdPool),
			"vkCreateCommandPool"
		);
	}

	void DestroyCmdPool(const vkDevice& mDevice, vkCmdPool* pCmdPool)
	{
		vkDestroyCommandPool(mDevice.GetDevice(), pCmdPool->m_CmdPool, NULL);
	}

	void AllocCmdBuf(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, vkCmdBuf& mCmdBuf)
	{
		VkCommandBufferAllocateInfo allocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = mCmdPool.GetCmdPool(),
			.level = mLevel,
			.commandBufferCount = 1u
		};

		GPU_CHECK(
			vkAllocateCommandBuffers(mCmdPool.m_pDevice->GetDevice(), &allocInfo, &mCmdBuf.m_CmdBuf),
			"vkAllocateCommandBuffers"
		);
	}

	void AllocCmdBufs(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, std::span<vkCmdBuf> mCmdBufs)
	{
		VkCommandBufferAllocateInfo allocInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = mCmdPool.GetCmdPool(),
			.level = mLevel,
			.commandBufferCount = (uint32_t)mCmdBufs.size()
		};

		std::vector<VkCommandBuffer> Cmds(mCmdBufs.size());

		GPU_CHECK(
			vkAllocateCommandBuffers(mCmdPool.m_pDevice->GetDevice(), &allocInfo, Cmds.data()),
			"vkAllocateCommandBuffers"
		);

		for (uint32_t i = 0; i < mCmdBufs.size(); i++)
		{
			// # Check the cmd's state
			if (!mCmdBufs[i].IsUnInited())
			{
				GPU_LOG_ERROR("vkGPU: vkCmdPool You'r trying to allocate vkCmdBuf is already allocated");
				exit(1);
			}
			// # Create the cmd and changing his state
			mCmdBufs[i].m_CmdBuf = Cmds[i];
			mCmdBufs[i].SetReady();
		}
	}

	void FreeCmdBufs(const vkCmdPool& mCmdPool, std::span<vkCmdBuf> mCmdBufs)
	{
		std::vector<VkCommandBuffer> Cmds(mCmdBufs.size());

		for (uint32_t i = 0; i < mCmdBufs.size(); i++)
		{
			// # Check the cmd's state
			if (mCmdBufs[i].IsUnInited())
			{
				GPU_LOG_ERROR("vkGPU: vkCmdPool You'r trying to free vkCmdBuf is uninitialized");
				exit(1);
			}
			// # Add the cmd & changing his state
			Cmds[i] = mCmdBufs[i].m_CmdBuf;
			mCmdBufs[i].SetUnInited();
		}

		// # Free the cmd
		vkFreeCommandBuffers(
			mCmdPool.m_pDevice->GetDevice(),
			mCmdPool.GetCmdPool(), (uint32_t)mCmdBufs.size(),
			Cmds.data()
		);
	}

	void BeginCmdBuf(const vkCmdBuf& mCmdBuf, VkCommandBufferUsageFlags mUsageFlags)
	{
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = mUsageFlags,
			.pInheritanceInfo = NULL
		};

		GPU_CHECK(
			vkBeginCommandBuffer(mCmdBuf.GetCmdBuf(), &beginInfo), 
			"vkBeginCommandBuffer"
		);
	}

	void EndCmdBuf(const vkCmdBuf& mCmdBuf)
	{
		GPU_CHECK(vkEndCommandBuffer(mCmdBuf.GetCmdBuf()), "vkEndCommandBuffer");
	}
}