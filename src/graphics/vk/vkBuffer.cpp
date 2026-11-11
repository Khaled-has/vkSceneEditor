#include "vkBuffer.h"
#include "vkBuffer.h"
#include "vkBuffer.h"
#include "vkBuffer.h"
#include "vkLog.h"

namespace GPU
{
	void CopyBuffer(VkBufferCopy mRegion, const vkCmdBuf& mCmdBuf, const vkBuffer& mSrc, vkBuffer& mDst)
	{
		CopyBuffer(std::span{ &mRegion, 1 }, mCmdBuf, mSrc, mDst);
	}

	void CopyBuffer(std::span<VkBufferCopy> mRegions, const vkCmdBuf& mCmdBuf, const vkBuffer& mSrc, vkBuffer& mDst)
	{
		GPU_CHECK(vkCmdCopyBuffer(
			mCmdBuf.GetCmdBuf(), 
			mSrc.GetBuffer(), mDst.GetBuffer(), 
			(uint32_t)mRegions.size(), mRegions.data()
		), "vkCmdCopyBuffer");
	}

	void CreateBuffer(const vkBufferCreateInfo& mInfo, vkBuffer* pBuffer)
	{
		pBuffer->m_BufferSize = (VkDeviceSize)mInfo.mSize;

		VkBufferCreateInfo BufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = (VkDeviceSize)mInfo.mSize,
			.usage = mInfo.mUsage
		};

		GPU_CHECK(
			vmaCreateBuffer(
			mInfo.mAllocator,
			&BufferInfo, mInfo.mAllocInfo,
			&pBuffer->m_Buffer,
			&pBuffer->m_Allocation,
			&pBuffer->m_AllocInfo
		), 
			"vmaCreateBuffer"
		);

		pBuffer->m_Allocator = mInfo.mAllocator;
	}

	void DestroyBuffer(vkBuffer* pBuffer)
	{
		vmaDestroyBuffer(pBuffer->m_Allocator, pBuffer->m_Buffer, pBuffer->m_Allocation);
	}

	//void AllocateStorageBuffer(vkBuffer& mBuffer, const void* pData, size_t mSrcOffset, size_t mDstOffset, size_t mSize, const vkCmdBuf& mCmdBuf)
	//{
	//	// # Create the stageing buffer
	//	VkBufferCreateInfo stagingBufferInfo = {
	//		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	//		.size = (VkDeviceSize)mSize,
	//		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
	//	};

	//	VmaAllocationCreateInfo stagingAllocInfo = {
	//		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
	//		.usage = VMA_MEMORY_USAGE_AUTO
	//	};

	//	vkBuffer stagingBuffer;
	//	stagingBuffer.Create(stagingBufferInfo, stagingAllocInfo);

	//	// # Memory copy to the staging buffer
	//	void* ptrData;
	//	vmaMapMemory(GetCurrentContext()->getVmaRes().mAllocator, stagingBuffer.getAllocation(), &ptrData);
	//	
	//	memcpy(ptrData, pData, mSize);

	//	vmaUnmapMemory(GetCurrentContext()->getVmaRes().mAllocator, stagingBuffer.getAllocation());

	//	// # Create the dst buffer
	//	VkBufferCreateInfo bufferInfo = {
	//		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	//		.size = (VkDeviceSize)mSize,
	//		.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
	//	};

	//	VmaAllocationCreateInfo allocInfo = {
	//		.usage = VMA_MEMORY_USAGE_AUTO,
	//	};

	//	mBuffer.Create(bufferInfo, allocInfo);

	//	// # Copy the staging buffer to the dst buffer
	//	VkBufferCopy region = {
	//		.srcOffset = (VkDeviceSize)mSrcOffset,
	//		.dstOffset = (VkDeviceSize)mDstOffset,
	//		.size = (VkDeviceSize)mSize
	//	};
	//	
	//	BeginCmdBuf(mCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	//	
	//	CopyBufferToBuffer(stagingBuffer, mBuffer, region, mCmdBuf);
	//	
	//	EndCmdBuf(mCmdBuf);

	//	const vkQueue* pQueue = GetCurrentContext()->getDeviceRes().mDeviceQueues.Transfer;
	//	vkCmdBuf CmdBufs[]{ mCmdBuf };

	//	SubmitSync(*pQueue, CmdBufs);
	//	WaitIdle(*pQueue);
	//}
}