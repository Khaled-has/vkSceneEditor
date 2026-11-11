#pragma once

#include "vkConfig.h"
#include "vkDevice.h"

namespace GPU
{
	// # Command buffer

	class VKGPU_API vkCmdBuf
	{
		friend class vkCmdPool;

		friend void AllocCmdBuf(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, vkCmdBuf& mCmdBuf);
		friend void AllocCmdBufs(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, std::span<vkCmdBuf> mCmdBufs);
		friend void FreeCmdBufs(const vkCmdPool& mCmdPool, std::span<vkCmdBuf> mCmdBufs);

		friend void BeginCmdBuf(const vkCmdBuf& mCmdBuf, VkCommandBufferUsageFlags mUsageFlags);
		friend void EndCmdBuf(const vkCmdBuf& mCmdBuf);
	public:
		vkCmdBuf() {}

		inline const VkCommandBuffer& GetCmdBuf() const { return m_CmdBuf; }

		inline bool IsUnInited()	const { return m_State == CmdState::UN_INITED ? true : false; }
		inline bool IsReady()		const { return m_State == CmdState::ON_READY ? true : false; }
		inline bool IsBegin()		const { return m_State == CmdState::ON_BEGIN ? true : false; }
		inline bool IsEnded()		const { return m_State == CmdState::ON_ENDED ? true : false; }
		inline bool IsSubmited()	const { return m_State == CmdState::ON_SUBMITED ? true : false; }
		
	private:
		inline void SetUnInited()	{ m_State = CmdState::UN_INITED; }
		inline void SetReady()		{ m_State = CmdState::ON_READY; }
		inline void SetBegin()		{ m_State = CmdState::ON_BEGIN; }
		inline void SetEnded()		{ m_State = CmdState::ON_ENDED; }
		inline void SetSubmited()   { m_State = CmdState::ON_SUBMITED; }

	private:
		VkCommandBuffer m_CmdBuf = VK_NULL_HANDLE;

		enum class CmdState
		{
			UN_INITED,
			ON_READY,
			ON_BEGIN,
			ON_ENDED,
			ON_SUBMITED
		};
		CmdState m_State = CmdState::UN_INITED;
	};

	// # Command pool
	struct vkCmdPoolCreateInfo
	{
		VkCommandPoolCreateFlags mFlags;
		uint32_t mQueueFamilyIndex;
	};

	class VKGPU_API vkCmdPool
	{
		friend void CreateCmdPool(const vkCmdPoolCreateInfo& mInfo, const vkDevice& mDevice, vkCmdPool* pCmdPool);
		friend void DestroyCmdPool(const vkDevice& mDevice, vkCmdPool* pCmdPool);

		friend void AllocCmdBuf(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, vkCmdBuf& mCmdBuf);
		friend void AllocCmdBufs(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, std::span<vkCmdBuf> mCmdBufs);
		friend void FreeCmdBufs(const vkCmdPool& mCmdPool, std::span<vkCmdBuf> mCmdBufs);

	public:
		vkCmdPool() {}

		inline const VkCommandPool& GetCmdPool() const { return m_CmdPool; }

	private:
		VkCommandPool m_CmdPool = VK_NULL_HANDLE;
		const vkDevice* m_pDevice;
	};

	VKGPU_API void CreateCmdPool(const vkCmdPoolCreateInfo& mInfo, const vkDevice& mDevice, vkCmdPool* pCmdPool);
	VKGPU_API void DestroyCmdPool(const vkDevice& mDevice, vkCmdPool* pCmdPool);

	VKGPU_API void AllocCmdBuf(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, vkCmdBuf& mCmdBuf);
	VKGPU_API void AllocCmdBufs(const vkCmdPool& mCmdPool, VkCommandBufferLevel mLevel, std::span<vkCmdBuf> mCmdBufs);
	VKGPU_API void FreeCmdBufs(const vkCmdPool& mCmdPool, std::span<vkCmdBuf> mCmdBufs);

	VKGPU_API void BeginCmdBuf(const vkCmdBuf& mCmdBuf, VkCommandBufferUsageFlags mUsageFlags);
	VKGPU_API void EndCmdBuf(const vkCmdBuf& mCmdBuf);
}
