#include "RenderGraph.h"
#include <log.h>

void RenderGraph::Setup(const RenderResources& mResources, const GPU::vkQueue& mQueue)
{
	// # Create render graph cmdbufs
	GPU::vkCmdPoolCreateInfo CmdPoolInfo = {
		.mFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.mQueueFamilyIndex = mQueue.GetQFamilyIndex()
	};
	GPU::CreateCmdPool(CmdPoolInfo, mResources.GetDevice(), &m_CmdPool);

	m_CmdBufs.resize(mResources.GetSwapChain().GetSwapImageCount());
	GPU::AllocCmdBufs(m_CmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_CmdBufs);

	// # Create the render graph resources
	m_RenderPassResources.resize(m_RenderPasses.size());

	// # Setup the render passes
	for (uint32_t i = 0; i < m_RenderPasses.size(); i++)
	{
		PassSetup Setup;
		m_RenderPasses[i]->Setup(Setup);

		// # Create new texture
		for (const auto& Im : Setup.m_AddImageCmds)
		{
			CreateNewImage(Im.mName, Im.mInfo);
		}

		// # Add the texture read commands
		for (uint32_t c = 0; c < Setup.m_ReadImages.size(); c++)
		{
			m_RenderPassResources[i].mReadImages.push_back(Setup.m_ReadImages[c]);
		}
		// # Add the texture write commands
		for (uint32_t c = 0; c < Setup.m_WriteImages.size(); c++)
		{
			m_RenderPassResources[i].mWriteImages.push_back(Setup.m_WriteImages[c]);
		}
	}

	// # Init the render passes
	for (uint32_t i = 0; i < m_RenderPasses.size(); i++)
	{
		PassInit Init;

		// # Write images
		for (uint32_t c = 0; c < m_RenderPassResources[i].mWriteImages.size(); c++)
		{
			std::string Name = m_RenderPassResources[i].mWriteImages[c];
			Init.m_WriteImages[Name] = m_Images[Name];
		}
		// # Read images
		for (uint32_t c = 0; c < m_RenderPassResources[i].mReadImages.size(); c++)
		{
			std::string Name = m_RenderPassResources[i].mReadImages[c];
			Init.m_ReadImages[Name] = m_Images[Name];
		}

		m_RenderPasses[i]->Init(mResources, Init);
	}
}

void RenderGraph::Destroy()
{
	for (RenderPass* Pass : m_RenderPasses)
	{
		Pass->Destroy();
	}

	for (auto& [Name, Image] : m_Images)
	{
		GPU::DestroyAll(m_Device, &Image);
	}
}

const GPU::vkCmdBuf& RenderGraph::Execute(uint32_t mSwapIndex)
{
	const GPU::vkCmdBuf& CmdBuf = m_CmdBufs[mSwapIndex];

	// # Begin recording
	vkResetCommandBuffer(CmdBuf.GetCmdBuf(), 0);
	GPU::BeginCmdBuf(CmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// # Reset the image layouts
	for (auto& [Name, Image] : m_Images)
	{
		GPU::ResetImageLayout(Image);
	}

	for (uint32_t i = 0; i < m_RenderPasses.size(); i++)
	{
		RenderPass* Pass = m_RenderPasses[i];

		std::vector<GPU::vkTexture> Images;
		std::vector<VkClearColorValue> Clears;

		for (uint32_t c = 0; c < m_RenderPassResources[i].mWriteImages.size(); c++)
		{
			GPU::TextureLayoutTransition(
				m_Images[m_RenderPassResources[i].mWriteImages[c]], CmdBuf,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			);

			Images.push_back(m_Images[m_RenderPassResources[i].mWriteImages[c]]);
			Clears.push_back(VkClearColorValue{ 0, 0, 0, 1 });
		}

		GPU::vkDynamicInfo DynamicInfo = {
			.mColorTextures = Images,
			.mClearColors = Clears,
			.mRenderArea = VkRect2D{.offset{0, 0}, .extent{1440, 720}}
		};

		// # Dynamic rendering for the graphics passes
		if (Pass->GetType() == GPU_RENDER_PASS_TYPE_GRAPHICS)
		{
			GPU::BeginDynamicRendering(CmdBuf, DynamicInfo);
		}

		Pass->Execute(CmdBuf, mSwapIndex);

		if (Pass->GetType() == GPU_RENDER_PASS_TYPE_GRAPHICS)
		{
			vkCmdEndRendering(CmdBuf.GetCmdBuf());
		}

		Images.clear();
		Clears.clear();
	}

	// # Transition the out images
	for (const auto& Im : m_OutImages)
	{
		GPU::TextureLayoutTransition(m_Images[Im.mName], CmdBuf, Im.mLayout);
	}

	// # End recording
	GPU::EndCmdBuf(CmdBuf);

	return CmdBuf;
}

const GPU::vkTexture* RenderGraph::GetImage(const std::string& mName, VkImageLayout mLayout)
{
	m_OutImages.push_back(OutImageInfo{ .mName = mName, .mLayout = mLayout });

	return &m_Images[mName];
}

void RenderGraph::CreateNewImage(const std::string& mName, ImageInfo mInfo)
{
	m_Images[mName] = GPU::vkTexture{};
	GPU::vkTexture& Image = m_Images[mName];

	VmaAllocationCreateInfo AllocInfo = {
		.usage = VMA_MEMORY_USAGE_AUTO
	};

	GPU::vkImageCreateInfo ImageInfo = {
		.mFormat = mInfo.mFormat,
		.mExtent = VkExtent3D{.width = mInfo.mExtent.width, .height = mInfo.mExtent.height, .depth = 1 },
		.mUsage = mInfo.mUsage,
		.mLayerCount = 1,

		.mAllocator = m_Allocator,
		.mAllocInfo = &AllocInfo
	};
	GPU::CreateImage(ImageInfo, &Image);

	GPU::vkViewCreateInfo ViewInfo = {
		.mViewType = VK_IMAGE_VIEW_TYPE_2D,
		.mAspect = mInfo.mAspect
	};
	GPU::CreateView(ViewInfo, m_Device, &Image);

	GPU::vkSamplerCreateInfo SamplerInfo = {
		.mMinFilter = VK_FILTER_LINEAR,
		.mMaxFilter = VK_FILTER_LINEAR,
		.mAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT
	};
	GPU::CreateSampler(SamplerInfo, m_Device, &Image);
}

const GPU::vkTexture& PassInit::GetImage_Write(const std::string& mName) const
{
	if (!m_WriteImages.contains(mName))
	{
		LOG_ERROR(
			"RenderGraph error: You call GetImage_Write() but the image {0} is not founded | You must forgot to add a write command in the pass setup function",
			mName
		);
		exit(1);
	}

	return m_WriteImages.at(mName);
}

const GPU::vkTexture& PassInit::GetImage_Read(const std::string& mName) const
{
	if (!m_ReadImages.contains(mName))
	{
		LOG_ERROR(
			"RenderGraph error: You call GetImage_Read() but the image {0} is not founded | You must forgot to add a read command in the pass setup function",
			mName
		);
		exit(1);
	}

	return m_ReadImages.at(mName);
}
