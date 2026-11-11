#include "vkRenderer.h"
#include "vkRenderTarget.h"

#include <log.h>
#include <imgui_impl_vulkan.h>

vkRenderer* vkRenderer::m_pInstance = nullptr;

void vkRenderer::Setup()
{
	m_pInstance = this;
	Log::Init();

	// # Initialize volk
	volkInitialize();

	// # 1: Create the instance
	uint32_t ApiVersion = 0;
	vkEnumerateInstanceVersion(&ApiVersion);

	std::vector<const char*> layers = {
		"VK_LAYER_KHRONOS_validation"
	};
	
	std::vector<const char*> extensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
	#ifdef WIN32
		"VK_KHR_win32_surface",
	#elif ANDROID
		"VK_KHR_android_surface",
	#elif __linux__
		"VK_KHR_xcb_surface",
	#endif

	#if defined (WIN32) || (__linux__)
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	#endif
	};

	GPU::vkInstanceCreateInfo instanceInfo = {
		.pAppName = "vkSceneEditor",
		.pEngineName = "vkSceneEditor",
		.mAppVersion = VK_MAKE_VERSION(1, 0, 0),
		.mEngineVersion = VK_MAKE_VERSION(1, 0, 0),
		.mApiVersion = ApiVersion,

		.mLayers = layers,
		.mExtensions = extensions
	};

	GPU::CreateInstance(instanceInfo, &m_Instance);

	// # 2: Create the surface
	_func_CreateVkSurface(m_Instance.GetInstance(), &m_Surface);

	// # 3: Enumerate the physical devices
	uint32_t PhysDevCount = GPU::GetPhysDevicesCount(m_Instance);

	std::vector<GPU::vkPhysDevice> PhysDevices(PhysDevCount);
	GPU::EnumeratePhysDevices(m_Instance, m_Surface, PhysDevices);

	// # 4: Create the device
	GPU::vkDeviceCreateInfo deviceInfo = {
		.pPhysDevice = &PhysDevices[0],
		.mEnableDynamicRendering = true,
		.mExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		}
	};

	GPU::CreateDevice(deviceInfo, &m_Device);

	// # Create vma allocator
	VmaVulkanFunctions vmaVulkanFunctions;

	VmaAllocatorCreateInfo AllocatorInfo = {
		.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT,
		.physicalDevice = m_Device.GetPhysDevice().GetPhysDevice(),
		.device = m_Device.GetDevice(),
		.instance = m_Instance.GetInstance(),
		.vulkanApiVersion = m_Instance.GetApiVersion(),
	};

	vmaImportVulkanFunctionsFromVolk(&AllocatorInfo, &vmaVulkanFunctions);

	AllocatorInfo.pVulkanFunctions = &vmaVulkanFunctions;

	vmaCreateAllocator(&AllocatorInfo, &m_Allocator);

	// # Get the device queues
	GPU::GetDeviceQueue(m_Device, VK_QUEUE_GRAPHICS_BIT, &m_GraphicsQueue);
	GPU::GetDeviceQueue(m_Device, VK_QUEUE_TRANSFER_BIT, &m_TransferQueue);

	// # 5: Create the swapchain
	GPU::vkSwapChainCreateInfo swapchainInfo = {
		.mMinSwapImageCount = 2,
		.mMaxSwapImageCount = 3,

		.mSurfaceFormat = VkSurfaceFormatKHR{.format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
		.mPresentMode = VK_PRESENT_MODE_FIFO_KHR,
		.mImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
	};

	GPU::CreateSwapChain(swapchainInfo, m_Device, m_Surface, &m_SwapChain);

	m_Frames.resize(m_SwapChain.GetSwapImageCount());

	// # 6: Create the swapchain commands
	GPU::vkCmdPoolCreateInfo cmdPoolInfo = {
		.mFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.mQueueFamilyIndex = m_GraphicsQueue.GetQFamilyIndex() 
	};

	GPU::CreateCmdPool(cmdPoolInfo, m_Device, &m_SwapCmdPool);

	for (auto& F : m_Frames)
	{
		GPU::AllocCmdBuf(m_SwapCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, F.mCmdBuf);
	}

	// # 7: Create the swapchain semaphores & fences
	for (auto& F : m_Frames)
	{
		GPU::CreateSyncSemaphore(m_Device, &F.mRenderSema);
		GPU::CreateSyncSemaphore(m_Device, &F.mPresentSema);

		GPU::CreateFence(m_Device, &F.mRenderFence, VK_FENCE_CREATE_SIGNALED_BIT);
	}

	CreatePipeline();

	m_ImGuiRenderer.Setup(
		m_Instance, m_Device, 
		m_SwapChain, m_SwapCmdPool, 
		m_GraphicsQueue, m_TransferQueue, 
		m_Allocator
	);

	std::cout << "Done\n";
}

void vkRenderer::Destroy()
{

}

void vkRenderer::Render()
{
	ImGui::Begin("Viewport");
	ImGui::Image(
		m_Image,
		ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y)
	);
	ImGui::End();
	// # ImGui render
	m_ImGuiRenderer.Render();

	// # Get the next image index
	uint32_t FrameIndex = m_FrameCount % m_SwapChain.GetSwapImageCount();
	FrameData& Frame = m_Frames[FrameIndex];

	vkWaitForFences(m_Device.GetDevice(), 1, &Frame.mRenderFence.GetFence(), VK_TRUE, UINT64_MAX);
	vkResetFences(m_Device.GetDevice(), 1, &Frame.mRenderFence.GetFence());

	uint32_t SwapChainImageIndex = 0;
	GPU::AcquireNextImage(m_Device, m_SwapChain, UINT64_MAX, &Frame.mPresentSema, NULL, &SwapChainImageIndex, NULL);

	// # Begin the cmd
	vkResetCommandBuffer(Frame.mCmdBuf.GetCmdBuf(), 0);
	GPU::BeginCmdBuf(Frame.mCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// # Transition the swapchain image layout to draw
	GPU::TransitionSwapImageToDraw(m_SwapChain, Frame.mCmdBuf, SwapChainImageIndex);
	
	// # End the cmd
	GPU::EndCmdBuf(Frame.mCmdBuf);

	// # Draw the render targets
	const GPU::vkTexture* OutImage = m_pRenderGraph->GetImage("GBuffer.BaseColor", VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	m_Image = (ImTextureID)ImGui_ImplVulkan_AddTexture(OutImage->GetSampler(), OutImage->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	// # Submit the commands to the graphics queue
	std::vector<const GPU::vkCmdBuf> Cmds;
	Cmds.push_back(Frame.mCmdBuf);
	ExecuteRenderTargets(SwapChainImageIndex, &Cmds);
	Cmds.push_back(m_ImGuiRenderer.GetCmdBuf(m_SwapChain, FrameIndex, SwapChainImageIndex));

	GPU::vkSubmitInfo SubmitInfo = {
		.pWaitSemaphore = &Frame.mPresentSema,
		.mWaitDstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,

		.mCmds = Cmds,

		.pSignalSemaphore = &Frame.mRenderSema
	};

	GPU::SubmitQueue(m_GraphicsQueue, SubmitInfo, &Frame.mRenderFence);

	// # Present the queue
	VkPresentInfoKHR PresentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,

		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &Frame.mRenderSema.GetSemaphore(),

		.swapchainCount = 1,
		.pSwapchains = &m_SwapChain.GetSwapchain(),
		.pImageIndices = &SwapChainImageIndex
	};

	vkQueuePresentKHR(m_GraphicsQueue.GetQueue(), &PresentInfo);

	m_FrameCount++;
}

void vkRenderer::SubmitRenderTarget(gfx::RenderTarget* pRenderTarget)
{
	m_RenderTargets.push_back(pRenderTarget);
}

void vkRenderer::CreateBuffers()
{
	// # Create buffer
	const std::vector<float> Vertices = {
		-0.5f,  0.5f,
		 0.5f,  0.5f,
		 0.0f, -0.5f
	};

	GPU::vkBuffer StagingBuf;
	VmaAllocationCreateInfo StaingAllocInfo = {
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO,
	};
	size_t BufSize = size_t(Vertices.size() * sizeof(float));
	GPU::vkBufferCreateInfo StagingBuffInfo = {
		.mSize = BufSize,
		.mUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,

		.mAllocInfo = &StaingAllocInfo,
		.mAllocator = m_Allocator,
	};

	GPU::CreateBuffer(StagingBuffInfo, &StagingBuf);

	void* pData;
	vmaMapMemory(m_Allocator, StagingBuf.GetAllocation(), &pData);

	memcpy(pData, Vertices.data(), BufSize);

	vmaUnmapMemory(m_Allocator, StagingBuf.GetAllocation());

	GPU::vkBuffer Buffer;

	VmaAllocationCreateInfo AllocInfo{
		.usage = VMA_MEMORY_USAGE_AUTO
	};
	GPU::vkBufferCreateInfo BufInfo
	{
		.mSize = BufSize,
		.mUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

		.mAllocInfo = &AllocInfo,
		.mAllocator = m_Allocator
	};
	GPU::CreateBuffer(BufInfo, &Buffer);

	GPU::vkCmdBuf CmdBuf;
	GPU::AllocCmdBuf(m_SwapCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, CmdBuf);

	GPU::BeginCmdBuf(CmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy Region = {
		.srcOffset = 0,
		.dstOffset = 0,
		.size = BufSize,
	};

	GPU::CopyBuffer(Region, CmdBuf, StagingBuf, Buffer);

	GPU::EndCmdBuf(CmdBuf);

	GPU::vkSubmitInfo SubmitInfo = {
		.pCmd = &CmdBuf
	};

	GPU::SubmitQueue(m_TransferQueue, SubmitInfo, NULL);

	// # Add buffer to the render resources
	m_pRenderResources->AddBufferResource("Vertices", Buffer);
}

void vkRenderer::CreateDescriptors()
{
	GPU::vkDescriptorPool DescPool;
	VkDescriptorSetLayout DescSetLayout;
	GPU::vkDescriptorSet DescSet;

	VkDescriptorPoolSize PoolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 }
	};

	GPU::CreateDescPool(m_Device, PoolSizes, 1, &DescPool);

	VkDescriptorSetLayoutBinding Binding = {
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
	};

	VkDescriptorSetLayoutCreateInfo DescLayoutInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = 1,
		.pBindings = &Binding
	};

	vkCreateDescriptorSetLayout(m_Device.GetDevice(), &DescLayoutInfo, NULL, &DescSetLayout);

	GPU::AddDescBinding(Binding, &DescSet);
	GPU::AllocateDescSet(m_Device, DescPool, DescSetLayout, &DescSet);
	VkDescriptorBufferInfo DescBufferInfo = {
		.buffer = m_pRenderResources->GetBufferResource("Vertices").GetBuffer(),
		.offset = 0,
		.range = VK_WHOLE_SIZE
	};

	VkWriteDescriptorSet WriteDesc = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.dstSet = DescSet.GetDescSet(),
		.dstBinding = 0,
		.dstArrayElement = 0,
		.descriptorCount = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.pBufferInfo = &DescBufferInfo
	};

	vkUpdateDescriptorSets(m_Device.GetDevice(), 1, &WriteDesc, 0, NULL);

	DescriptorResource DescResource = {
		.mDescLayout = DescSetLayout,
		.mDescSet = DescSet
	};

	// # Add the descriptor to the render resources
	m_pRenderResources->AddDescriptorResource("Descriptor_0", DescResource);
}

void vkRenderer::CreateShaders()
{
	struct ShaderInfo {
		const char* pName;
		const char* pPath;

		std::vector<LPCWSTR> mArguments;
	};

	std::vector<ShaderInfo> ShaderInfos = {
		// # geometry shader
		{
			.pName = "geometry.vert",
			.pPath = "shaders/geometry.vert.hlsl",

			.mArguments = { L"geometry.vert", L"-E", L"main", L"-T", L"vs_6_1", L"-spirv" }
		},
		{
			.pName = "geometry.frag",
			.pPath = "shaders/geometry.frag.hlsl",

			.mArguments = { L"geometry.frag", L"-E", L"main", L"-T", L"ps_6_1", L"-spirv" }
		}
	};

	for (uint32_t i = 0; i < ShaderInfos.size(); i++)
	{
		GPU::vkShader Shader;
		GPU::vkShaderLoadInfo_dxc Info = { .mArguments = ShaderInfos[i].mArguments };
		GPU::LoadShader_dxc(m_Device, ShaderInfos[i].pPath, Info, &Shader);

		// # Add shader to the render resources
		m_pRenderResources->AddShaderResource(ShaderInfos[i].pName, Shader);
	}
}

void vkRenderer::CreateResources()
{
	// # Create buffers
	CreateBuffers();
	// # Create descriptor sets
	CreateDescriptors();
	// # Create shaders
	m_DxcCompiler.Create();
	GPU::SetDxcContext(m_DxcCompiler.GetUtils(), m_DxcCompiler.GetCompiler());
	CreateShaders();
}

void vkRenderer::CreatePipeline()
{
	// # Create the pipeline resources
	m_pRenderResources = std::make_unique<RenderResources>(m_Device, m_Allocator, m_SwapChain);
	CreateResources();

	m_pRenderGraph = std::make_unique<RenderGraph>(m_Device, m_Allocator);
	m_pRenderGraph->AddRenderPass<GeometryPass>();

	m_pRenderGraph->Setup(*m_pRenderResources, m_GraphicsQueue);
}

void vkRenderer::ExecuteRenderTargets(uint32_t mSwapIndex, std::vector<const GPU::vkCmdBuf>* mCmdBufs)
{
	// # Execute the sended render target
	for (gfx::RenderTarget* R : m_RenderTargets)
	{
		const GPU::vkCmdBuf& Cmd = reinterpret_cast<vkRenderTarget*>(R)->Execute(mSwapIndex);
		mCmdBufs->push_back(Cmd);
	}

	// # Clear the render target
	m_RenderTargets.clear();
}
