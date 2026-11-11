#ifndef VKRENDERER_H
#define VKRENDERER_H

#include "Renderer.h"

#include <vkGPU.h>
#include "Compilers/DxCompiler.h"
#include "ImGui/ImGuiRenderer.h"
#include "Passes/GeometryPass.h"

#include "ImGui/ImGuiImage.h"

struct FrameData
{
	GPU::vkSemaphore mRenderSema;
	GPU::vkSemaphore mPresentSema;
	GPU::vkFence mRenderFence;

	GPU::vkCmdBuf mCmdBuf;
};

class vkRenderer : public gfx::Renderer
{
public:
	vkRenderer(std::function<void (void* pInstance, void* pSurface)> func_CreateVkSurface) : _func_CreateVkSurface(func_CreateVkSurface) {}

	virtual void Setup() override;
	virtual void Destroy() override;

	virtual void Render() override;

	virtual void SubmitRenderTarget(gfx::RenderTarget* pRenderTarget) override;

public:
	static const RenderResources& GetRenderResources()	{ return *m_pInstance->m_pRenderResources; }

	static const GPU::vkQueue& GetGraphicsQueue()		{ return m_pInstance->m_GraphicsQueue; }
	static const GPU::vkQueue& GetTransferQueue()		{ return m_pInstance->m_TransferQueue; }
private:
	static vkRenderer* m_pInstance;
	std::vector<gfx::RenderTarget*> m_RenderTargets;

	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	std::function<void (void* pInstance, void* pSurface)> _func_CreateVkSurface;

	GPU::vkInstance m_Instance;

	GPU::vkPhysDevice m_SelectedPhysDevice;
	GPU::vkDevice m_Device;
	VmaAllocator m_Allocator;

	GPU::vkSwapChain m_SwapChain;
	GPU::vkCmdPool m_SwapCmdPool;

	std::vector<FrameData> m_Frames;
	int m_FrameCount = 0;

	GPU::vkQueue m_GraphicsQueue;
	GPU::vkQueue m_TransferQueue;

	DxCompiler m_DxcCompiler;

	ImGuiRenderer m_ImGuiRenderer;

	// # Passes
	std::unique_ptr<RenderGraph> m_pRenderGraph;
	std::unique_ptr<RenderResources> m_pRenderResources;

	void CreateBuffers();
	void CreateDescriptors();
	void CreateShaders();
	void CreateResources();

	void CreatePipeline();

	void ExecuteRenderTargets(uint32_t mSwapIndex, std::vector<const GPU::vkCmdBuf>* mCmdBufs);

	ImTextureID m_Image;
};

#endif