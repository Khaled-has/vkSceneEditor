#ifndef IMGUIRENDERER_H
#define IMGUIRENDERER_H

#include <vkGPU.h>

class ImGuiRenderer
{
public:
	ImGuiRenderer() {}

	void Setup(
		const GPU::vkInstance& mInstance, const GPU::vkDevice& mDevice, const GPU::vkSwapChain& mSwapChain, const GPU::vkCmdPool& mCmdPool, 
		const GPU::vkQueue& mGraphicsQueue, const GPU::vkQueue& mTransferQueue, VmaAllocator mAllocator
	);

	const GPU::vkCmdBuf& GetCmdBuf(GPU::vkSwapChain& mSwapChain, uint32_t mFrameIndex, uint32_t mSwapIndex);
	void Render();

public:
	static inline const GPU::vkDevice& GetDevice()		{ return *m_pInstance->m_pDevice; }
	static inline const GPU::vkQueue& GetQueue()		{ return *m_pInstance->m_pTransferQueue; }
	static inline VmaAllocator GetAllocator()			{ return m_pInstance->m_Allocator; }
	static inline const GPU::vkCmdPool& GetCmdPool()	{ return m_pInstance->m_CmdPool; }

private:
	static ImGuiRenderer* m_pInstance;

	const GPU::vkDevice* m_pDevice;
	const GPU::vkQueue* m_pTransferQueue;
	VmaAllocator m_Allocator;

	GPU::vkCmdPool m_CmdPool;
	std::vector<GPU::vkCmdBuf> m_CmdBufs;

	GPU::vkDescriptorPool m_DescPool;
	VkRect2D m_RenderArea;

	void ImGuiStyleInit();
};

#endif