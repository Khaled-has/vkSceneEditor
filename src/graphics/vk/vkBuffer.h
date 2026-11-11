#pragma once

#include "vkCmdBuf.h"

namespace GPU
{
	struct vkBufferCreateInfo
	{
		size_t mSize;
		VkBufferUsageFlags mUsage;

		const VmaAllocationCreateInfo* mAllocInfo;
		VmaAllocator mAllocator;
	};

	class VKGPU_API vkBuffer
	{
		friend void CopyBuffer(VkBufferCopy mRegion, const vkCmdBuf& mCmdBuf, const vkBuffer& mSrc, vkBuffer& mDst);
		friend void CopyBuffer(std::span<VkBufferCopy> mRegions, const vkCmdBuf& mCmdBuf, const vkBuffer& mSrc, vkBuffer& mDst);

		friend void CreateBuffer(const vkBufferCreateInfo& mInfo, vkBuffer* pBuffer);
		friend void DestroyBuffer(vkBuffer* pBuffer);
	public:
		vkBuffer() {}

		inline const VkBuffer& GetBuffer()				const { return m_Buffer; }
		inline const VmaAllocation& GetAllocation()		const { return m_Allocation; }
		inline const VmaAllocationInfo& GetAllocInfo()	const { return m_AllocInfo; }

		inline VkDeviceSize GetBufferSize()				const { return m_BufferSize; }
		inline VmaAllocationInfo GetVmaAllocInfo()		const { return m_AllocInfo; }
	private:
		VkBuffer m_Buffer			= VK_NULL_HANDLE;
		VmaAllocation m_Allocation	= VK_NULL_HANDLE;
		VkDeviceSize m_BufferSize	= 0;
		VmaAllocationInfo m_AllocInfo;

		VmaAllocator m_Allocator;
	};

	VKGPU_API void CopyBuffer(VkBufferCopy mRegion, const vkCmdBuf& mCmdBuf, const vkBuffer& mSrc, vkBuffer& mDst);
	VKGPU_API void CopyBuffer(std::span<VkBufferCopy> mRegions, const vkCmdBuf& mCmdBuf, const vkBuffer& mSrc, vkBuffer& mDst);

	VKGPU_API void CreateBuffer(const vkBufferCreateInfo& mInfo, vkBuffer* pBuffer);
	VKGPU_API void DestroyBuffer(vkBuffer* pBuffer);
}
