#ifndef VKDESCRIPTORSET_H
#define VKDESCRIPTORSET_H

#include "vkDevice.h"

namespace GPU
{
	// # Descriptor pool
	class VKGPU_API vkDescriptorPool
	{
		friend void CreateDescPool(const vkDevice& mDevice, std::span<const VkDescriptorPoolSize> mPoolSizes, uint32_t mMaxSets, vkDescriptorPool* pDescPool);
	public:
		vkDescriptorPool() {}

		inline const VkDescriptorPool& GetDescPool() const { return m_DescPool; }

	private:
		VkDescriptorPool m_DescPool = VK_NULL_HANDLE;
	};

	// # Descriptor set
	class VKGPU_API vkDescriptorSet
	{
		friend void AddDescBinding(VkDescriptorSetLayoutBinding mBind, vkDescriptorSet* pDescSet);
		friend void AddDescBindings(std::span<const VkDescriptorSetLayoutBinding> mBinds, vkDescriptorSet* pDescSet);

		friend void AllocateDescSet(const vkDevice& mDevice, const vkDescriptorPool& mDescPool, VkDescriptorSetLayout mDescLayout, vkDescriptorSet* pDescSet);
		friend void AllocateDescSets(const vkDevice& mDevice, const vkDescriptorPool& mDescPool, std::span<const VkDescriptorSetLayout> mDescLayouts, std::span<vkDescriptorSet> mDescSets);
	public:
		vkDescriptorSet() {}

		inline const VkDescriptorSet& GetDescSet()								const { return m_DescSet; }
		inline const std::vector<VkDescriptorSetLayoutBinding>& GetBindings()	const { return m_Bindings; }

	private:
		VkDescriptorSet m_DescSet = VK_NULL_HANDLE;
		std::vector<VkDescriptorSetLayoutBinding> m_Bindings;
	};

	VKGPU_API void CreateDescPool(const vkDevice& mDevice, std::span<const VkDescriptorPoolSize> mPoolSizes, uint32_t mMaxSets, vkDescriptorPool* pDescPool);

	VKGPU_API void AddDescBinding(VkDescriptorSetLayoutBinding mBind, vkDescriptorSet* pDescSet);
	VKGPU_API void AddDescBindings(std::span<const VkDescriptorSetLayoutBinding> mBinds, vkDescriptorSet* pDescSet);

	VKGPU_API void AllocateDescSet(const vkDevice& mDevice, const vkDescriptorPool& mDescPool, VkDescriptorSetLayout mDescLayout, vkDescriptorSet* pDescSet);
	VKGPU_API void AllocateDescSets(const vkDevice& mDevice, const vkDescriptorPool& mDescPool, std::span<const VkDescriptorSetLayout> mDescLayouts, std::span<vkDescriptorSet> mDescSets);
}

#endif