#include "ImGuiRenderer.h"
#include <log.h>
#include <imgui_impl_vulkan.h>

void CheckVkResult(VkResult res)
{
	if (res == 0)
	{
		return;
	}

	LOG_INFO("vkImGui: ImGui create info, {0}", string_VkResult(res));

	if (res < 0)
	{
		abort();
	}
}

ImGuiRenderer* ImGuiRenderer::m_pInstance = nullptr;

void ImGuiRenderer::Setup(
	const GPU::vkInstance& mInstance, const GPU::vkDevice& mDevice, const GPU::vkSwapChain& mSwapChain, const GPU::vkCmdPool& mCmdPool,
	const GPU::vkQueue& mGraphicsQueue, const GPU::vkQueue& mTransferQueue, VmaAllocator mAllocator
)
{
	m_pInstance = this;

	m_pDevice = &mDevice;
	m_pTransferQueue = &mTransferQueue;
	m_Allocator = mAllocator;
	m_RenderArea = mSwapChain.GetSwapExtent();

	// # Init style
	ImGuiStyleInit();

	// # Create transfer cmd pool
	GPU::vkCmdPoolCreateInfo CmdPoolInfo = {
		.mFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.mQueueFamilyIndex = mTransferQueue.GetQFamilyIndex()
	};
	GPU::CreateCmdPool(CmdPoolInfo, mDevice, &m_CmdPool);

	// # Allocate command buffers
	m_CmdBufs.resize(mSwapChain.GetSwapImageCount());
	GPU::AllocCmdBufs(mCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_CmdBufs);

	// # Create descriptor pool
	VkDescriptorPoolSize PoolSizes[] = {
	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	GPU::CreateDescPool(mDevice, PoolSizes, 1000, &m_DescPool);

	// # Initialize imgui
	VkFormat Format = mSwapChain.GetSurfaceFormat().format;
	ImGui_ImplVulkan_InitInfo InitInfo = {
		.ApiVersion = mInstance.GetApiVersion(),
		.Instance = mInstance.GetInstance(),
		.PhysicalDevice = mDevice.GetPhysDevice().GetPhysDevice(),
		.Device = mDevice.GetDevice(),
		.QueueFamily = mGraphicsQueue.GetQFamilyIndex(),
		.Queue = mGraphicsQueue.GetQueue(),
		.DescriptorPool = m_DescPool.GetDescPool(),
		.MinImageCount = mDevice.GetPhysDevice().GetInfo().mSurfaceCaps.minImageCount,
		.ImageCount = mSwapChain.GetSwapImageCount(),
		.PipelineInfoMain = {
			.RenderPass = NULL,
			.Subpass = 0,
			.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
			.PipelineRenderingCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
				.pNext = NULL,
				.viewMask = 0,
				.colorAttachmentCount = 1,
				.pColorAttachmentFormats = &Format,
				.depthAttachmentFormat = VK_FORMAT_UNDEFINED,
				.stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
			},
		},
		.UseDynamicRendering = true,
		.Allocator = NULL,
		.CheckVkResultFn = CheckVkResult
	};

	ImGui_ImplVulkan_LoadFunctions(mInstance.GetApiVersion(), [](const char* function_name, void* user_data)
		{
			VkInstance instance = volkGetLoadedInstance();
			return vkGetInstanceProcAddr(instance, function_name);
		}
	);

	ImGui_ImplVulkan_Init(&InitInfo);
}

const GPU::vkCmdBuf& ImGuiRenderer::GetCmdBuf(GPU::vkSwapChain& mSwapChain, uint32_t mFrameIndex, uint32_t mSwapIndex)
{
	vkResetCommandBuffer(m_CmdBufs[mSwapIndex].GetCmdBuf(), 0);
	GPU::BeginCmdBuf(m_CmdBufs[mSwapIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// # Begin rendering
	const VkClearColorValue Clear{ {0, 0, 0, 1} };

	GPU::vkDynamicInfo DynamicInfo = {
		.mColorTextures = std::span{&mSwapChain.GetSwapTextures()[mSwapIndex], 1},
		.mClearColors = std::span{ &Clear, 1 },
		.mRenderArea = m_RenderArea
	};
	GPU::BeginDynamicRendering(m_CmdBufs[mSwapIndex], DynamicInfo);

	ImDrawData* pDrawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(pDrawData, m_CmdBufs[mSwapIndex].GetCmdBuf());
	
	// # End rendering
	vkCmdEndRendering(m_CmdBufs[mSwapIndex].GetCmdBuf());

	// # Transition the swapchain texture to present
	GPU::TransitionSwapImageToPresent(mSwapChain, m_CmdBufs[mSwapIndex], mSwapIndex);

	// # End cmdbuf
	GPU::EndCmdBuf(m_CmdBufs[mSwapIndex]);

	return m_CmdBufs[mSwapIndex];
}

void ImGuiRenderer::Render()
{
	ImGui::Render();
	ImGui::EndFrame();

#ifdef WIN32
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
#endif
}

void ImGuiRenderer::ImGuiStyleInit()
{
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.DisplaySize.x = (float)m_RenderArea.extent.width;
	io.DisplaySize.y = (float)m_RenderArea.extent.height;
	
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}