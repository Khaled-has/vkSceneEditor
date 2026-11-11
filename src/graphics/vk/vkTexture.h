#ifndef VKTEXTURE_H
#define VKTEXTURE_H

#include "vkDevice.h"
#include "vkBuffer.h"

namespace GPU
{
	struct vkImageCreateInfo
	{
		VkFormat mFormat; 
		VkExtent3D mExtent;
		VkImageUsageFlags mUsage;
		uint32_t mLayerCount;

		VmaAllocator mAllocator;
		const VmaAllocationCreateInfo* mAllocInfo;
	};

	struct vkViewCreateInfo
	{
		VkImageViewType mViewType;
		VkImageAspectFlags mAspect;
	};

	struct vkSamplerCreateInfo
	{
		VkFilter mMinFilter;
		VkFilter mMaxFilter;
		VkSamplerAddressMode mAddressMode;
	};

	class VKGPU_API vkTexture
	{
		// # vkTexture components creation
		friend void CreateImage(const vkImageCreateInfo& mInfo, vkTexture* pTexture);
		friend void CreateView(const vkViewCreateInfo& mInfo, const vkDevice& mDevice, vkTexture* pTexture);
		friend void CreateSampler(const vkSamplerCreateInfo& mInfo, const vkDevice& mDevice, vkTexture* pTexture);

		// # vkTexture components destraction
		friend void DestroyImage(vkTexture* pTexture);
		friend void DestroyView(const vkDevice& mDevice, vkTexture* pTexture);
		friend void DestroySampler(const vkDevice& mDevice, vkTexture* pTexture);
		friend void DestroyAll(const vkDevice& mDevice, vkTexture* pTexture);

		friend void ResetImageLayout(vkTexture& mTexture, VkImageLayout mLayout);
		friend void TextureLayoutTransition(vkTexture& mTexture, const vkCmdBuf& mCmdBuf, VkImageLayout mNewLayout);
	public:
		vkTexture() {}

		// # Use this functions if you already have one of those components
		void SetImage(VkImage mImage, const vkImageCreateInfo& mInfo);
		void SetView(VkImageView mView, const vkViewCreateInfo& mInfo);
		void SetSampler(VkSampler mSampler, const vkSamplerCreateInfo& mInfo);

		// # vkTexture's components
		inline VkImage GetImage()		 const { return m_Image; }
		inline VkImageView GetView()	 const { return m_View; }
		inline VkSampler GetSampler()	 const { return m_Sampler; }
		inline VkImageLayout GetLayout() const { return m_Layout; }

		// # vkTexture components' info
		inline const vkImageCreateInfo& GetImageInfo()		const { return m_ImageInfo; }
		inline const vkViewCreateInfo& GetViewInfo()		const { return m_ViewInfo; }
		inline const vkSamplerCreateInfo& GetSamplerInfo()	const { return m_SamplerInfo; }

		// # Check vkTexture components creation 
		inline bool IsImageCreated()	const { return m_ImageCreated; }
		inline bool IsViewCreated()		const { return m_ViewCreated; }
		inline bool IsSamplerCreated()	const { return m_SamplerCreated; }

	private:
		VkImage m_Image		   = VK_NULL_HANDLE;
		VkImageView m_View	   = VK_NULL_HANDLE;
		VkSampler m_Sampler	   = VK_NULL_HANDLE;
		VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VmaAllocation m_Allocation;

		vkImageCreateInfo m_ImageInfo;
		vkViewCreateInfo m_ViewInfo;
		vkSamplerCreateInfo m_SamplerInfo;
		
		bool m_ImageCreated		= false;
		bool m_ViewCreated		= false;
		bool m_SamplerCreated	= false;
	};

	VKGPU_API void CreateImage(const vkImageCreateInfo& mInfo, vkTexture* pTexture);
	VKGPU_API void CreateView(const vkViewCreateInfo& mInfo, const vkDevice& mDevice, vkTexture* pTexture);
	VKGPU_API void CreateSampler(const vkSamplerCreateInfo& mInfo, const vkDevice& mDevice, vkTexture* pTexture);

	VKGPU_API void DestroyImage(vkTexture* pTexture);
	VKGPU_API void DestroyView(const vkDevice& mDevice, vkTexture* pTexture);
	VKGPU_API void DestroySampler(const vkDevice& mDevice, vkTexture* pTexture);
	VKGPU_API void DestroyAll(const vkDevice& mDevice, vkTexture* pTexture);

	VKGPU_API void ResetImageLayout(vkTexture& mTexture, VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED);
	VKGPU_API void TextureLayoutTransition(vkTexture& mTexture, const vkCmdBuf& mCmdBuf, VkImageLayout mNewLayout);

	VKGPU_API void CopyBufferToImage(const vkBuffer& mSrcBuf, const vkTexture& mDstTex, VkBufferImageCopy mRegion, const vkCmdBuf& mCmdBuf);
}

#endif