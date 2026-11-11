#ifndef RENDERGRAPH_H
#define RENDERGRAPH_H

#include <vkGPU.h>

typedef enum RenderPassType
{
	GPU_RENDER_PASS_TYPE_GRAPHICS = 0,
	GPU_RENDER_PASS_TYPE_COMPUTE = 1
} RenderPassType;

struct DescriptorResource
{
	VkDescriptorSetLayout mDescLayout;
	GPU::vkDescriptorSet mDescSet;
};

class RenderResources
{
public:
	RenderResources(
		const GPU::vkDevice& mDevice, VmaAllocator mAllocator, const GPU::vkSwapChain& mSwapChain
	)
		: m_Device(mDevice), m_Allocator(mAllocator), m_SwapChain(mSwapChain)
	{}

	inline const GPU::vkDevice& GetDevice()			const { return m_Device; }
	inline const VmaAllocator& GetAllocator()		const { return m_Allocator; }
	inline const GPU::vkSwapChain& GetSwapChain()	const { return m_SwapChain; }

public:
	// # Add resources
	inline void AddShaderResource(const std::string& mName, const GPU::vkShader& mShader)
	{
		m_ShaderResources[mName] = mShader;
	}

	inline void AddDescriptorResource(const std::string& mName, const DescriptorResource& mDescRes)
	{
		m_DescriptorResources[mName] = mDescRes;
	}

	inline void AddBufferResource(const std::string& mName, const GPU::vkBuffer& mBuffer)
	{
		m_BufferResources[mName] = mBuffer;
	}

	// # Get resources
	inline const GPU::vkShader& GetShaderResource(const std::string mName) const
	{
		return m_ShaderResources.at(mName);
	}

	inline const DescriptorResource& GetDescriptorResource(const std::string mName) const
	{
		return m_DescriptorResources.at(mName);
	}

	inline const GPU::vkBuffer& GetBufferResource(const std::string mName) const
	{
		return m_BufferResources.at(mName);
	}

private:
	const GPU::vkDevice& m_Device;
	VmaAllocator m_Allocator;
	const GPU::vkSwapChain& m_SwapChain;

	std::unordered_map<std::string, GPU::vkShader> m_ShaderResources;

	// # Remove letter to the render graph
	std::unordered_map<std::string, DescriptorResource> m_DescriptorResources;
	std::unordered_map<std::string, GPU::vkBuffer> m_BufferResources;
};

// # Render graph
struct ImageInfo
{
	VkImageUsageFlags mUsage;
	VkImageAspectFlags mAspect;
	VkFormat mFormat;
	VkExtent2D mExtent;
};

class PassSetup
{
	friend class RenderGraph;
public:
	PassSetup() {}

	inline void AddImage(const std::string& mName, ImageInfo mInfo) { m_AddImageCmds.push_back(AddImageCmd{ .mName = mName, .mInfo = mInfo}); }
	inline void WriteImage(const std::string& mName) { m_WriteImages.push_back(mName); }
	inline void ReadImage(const std::string& mName) { m_ReadImages.push_back(mName); }

private:
	struct AddImageCmd
	{
		std::string mName;
		ImageInfo mInfo;
	};

	std::vector<AddImageCmd> m_AddImageCmds;
	std::vector<std::string> m_WriteImages;
	std::vector<std::string> m_ReadImages;
};

class PassInit
{
	friend class RenderGraph;
public:
	PassInit() {}
	
	const GPU::vkTexture& GetImage_Write(const std::string& mName) const;
	const GPU::vkTexture& GetImage_Read(const std::string& mName) const;

private:
	std::unordered_map<std::string, GPU::vkTexture> m_WriteImages;
	std::unordered_map<std::string, GPU::vkTexture> m_ReadImages;
};

class RenderPass
{
public:
	RenderPass(RenderPassType mType, const std::string& mName = "UnNamed") 
		: m_PassType(mType), m_PassName(mName)
	{}

	virtual void Destroy() = 0;

	virtual void Setup(PassSetup& mSetup) = 0;
	virtual void Init(const RenderResources& mResources, PassInit& mInit) = 0;

	virtual void Execute(const GPU::vkCmdBuf& mCmdBuf, uint32_t mSwapIndex) = 0;

	inline const std::string& GetName() const { return m_PassName; }
	inline RenderPassType GetType()		const { return m_PassType; }

private:
	const std::string& m_PassName;
	RenderPassType m_PassType;
};

class RenderGraph
{
public:
	RenderGraph(const GPU::vkDevice& mDevice, VmaAllocator mAllocator) 
		: m_Device(mDevice), m_Allocator(mAllocator)
	{}

	template<typename T>
	inline void AddRenderPass() { m_RenderPasses.push_back(new T); }

	void Setup(const RenderResources& mResources, const GPU::vkQueue& mQueue);
	void Destroy();

	const GPU::vkCmdBuf& Execute(uint32_t mSwapIndex);

	const GPU::vkTexture* GetImage(const std::string& mName, VkImageLayout mLayout);

private:
	GPU::vkCmdPool m_CmdPool;
	std::vector<GPU::vkCmdBuf> m_CmdBufs;

	const GPU::vkDevice& m_Device;
	VmaAllocator m_Allocator;

	std::vector<RenderPass*> m_RenderPasses;

	struct PassResources
	{
		std::vector<std::string> mWriteImages;
		std::vector<std::string> mReadImages;
	};
	std::vector<PassResources> m_RenderPassResources;

	std::unordered_map<std::string, GPU::vkTexture> m_Images;

	struct OutImageInfo
	{
		std::string mName;
		VkImageLayout mLayout;
	};
	std::vector<OutImageInfo> m_OutImages;

	void CreateNewImage(const std::string& mName, ImageInfo mInfo);
};


#endif