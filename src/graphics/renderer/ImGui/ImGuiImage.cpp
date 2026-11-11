#include "ImGuiImage.h"
#include "ImGuiRenderer.h"
#include <stb_image.h>
#include <imgui_impl_vulkan.h>

static VkFormat FindFormat(int c)
{
	switch (c)
	{
	case 1:
		return VK_FORMAT_R8_SRGB;
	case 2:
		return VK_FORMAT_R8G8_SRGB;
	case 3:
		return VK_FORMAT_R8G8B8_SRGB;
	case 4:
		return VK_FORMAT_R8G8B8A8_SRGB;

	default:
		exit(0);
	}
}

void ImGui_CreateImage(const char* pFilename, ImGuiImage* pImage)
{
	// # Load the image data from file
	int w, h, c;
	stbi_uc* pData = stbi_load(pFilename, &w, &h, &c, STBI_default);

	pImage->m_ImageInfo = {
		.mWidth = (uint32_t)w,
		.mHeight = (uint32_t)h,
		.mChannels = (uint32_t)c
	};

	size_t Size = size_t(w * h * c);

	// # Create staging buffer
	VmaAllocationCreateInfo StaginAllocInfo = {
		.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
		.usage = VMA_MEMORY_USAGE_AUTO
	};

	GPU::vkBufferCreateInfo StagingInfo = {
		.mSize = Size,
		.mUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		
		.mAllocInfo = &StaginAllocInfo,
		.mAllocator = ImGuiRenderer::GetAllocator()
	};

	GPU::vkBuffer StagingBuffer;
	GPU::CreateBuffer(StagingInfo, &StagingBuffer);

	// # Copy the data to the staging buffer memory
	memcpy(StagingBuffer.GetAllocInfo().pMappedData, pData, Size);
	stbi_image_free(pData);

	// # Create the texture
	pImage->m_pImage = new GPU::vkTexture();
	GPU::vkTexture* Image = static_cast<GPU::vkTexture*>(pImage->m_pImage);

	VmaAllocationCreateInfo AllocInfo = {
		.usage = VMA_MEMORY_USAGE_AUTO
	};

	GPU::vkImageCreateInfo ImageInfo = {
		.mFormat = FindFormat(c),
		.mExtent = VkExtent3D{ (uint32_t)w, uint32_t(h), 1 },
		.mUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.mLayerCount = 1,

		.mAllocator = ImGuiRenderer::GetAllocator(),
		.mAllocInfo = &AllocInfo
	};

	GPU::CreateImage(ImageInfo, Image);

	GPU::vkViewCreateInfo ViewInfo = {
		.mViewType = VK_IMAGE_VIEW_TYPE_2D,
		.mAspect = VK_IMAGE_ASPECT_COLOR_BIT
	};

	GPU::CreateView(ViewInfo, ImGuiRenderer::GetDevice(), Image);

	GPU::vkSamplerCreateInfo SamplerInfo = {
		.mMinFilter = VK_FILTER_LINEAR,
		.mMaxFilter = VK_FILTER_LINEAR,
		.mAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT
	};

	GPU::CreateSampler(SamplerInfo, ImGuiRenderer::GetDevice(), Image);

	// # Copy the buffer data to image
	GPU::vkCmdBuf Cmd;
	GPU::AllocCmdBuf(ImGuiRenderer::GetCmdPool(), VK_COMMAND_BUFFER_LEVEL_PRIMARY, Cmd);

	GPU::BeginCmdBuf(Cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	GPU::TextureLayoutTransition(*Image, Cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkBufferImageCopy Region = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1 },
		.imageOffset = {0, 0, 0},
		.imageExtent = VkExtent3D{.width = (uint32_t)w, .height = (uint32_t)h, .depth = 1},
	};
	GPU::CopyBufferToImage(StagingBuffer, *Image, Region, Cmd);

	GPU::TextureLayoutTransition(*Image, Cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	GPU::EndCmdBuf(Cmd);

	// # Submit the command buffer
	GPU::vkFence Fence;
	GPU::CreateFence(ImGuiRenderer::GetDevice(), &Fence);

	GPU::vkSubmitInfo SubmitInfo = { .pCmd = &Cmd };
	GPU::SubmitQueue(ImGuiRenderer::GetQueue(), SubmitInfo, &Fence);

	// # Destroy staging buffer
	vkWaitForFences(ImGuiRenderer::GetDevice().GetDevice(), 1, &Fence.GetFence(), VK_TRUE, UINT64_MAX);
	GPU::DestroyBuffer(&StagingBuffer);

	pImage->m_TexID = (ImTextureID)ImGui_ImplVulkan_AddTexture(Image->GetSampler(), Image->GetView(), Image->GetLayout());
}
