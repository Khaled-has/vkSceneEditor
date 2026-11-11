#include "vkDescriptorSet.h"
#include "vkLog.h"

namespace GPU
{
	void CreateDescPool(const vkDevice& mDevice, std::span<const VkDescriptorPoolSize> mPoolSizes, uint32_t mMaxSets, vkDescriptorPool* pDescPool)
	{
		VkDescriptorPoolCreateInfo Info = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = mMaxSets * (uint32_t)mPoolSizes.size(),
			.poolSizeCount = (uint32_t)mPoolSizes.size(),
			.pPoolSizes = mPoolSizes.data()
		};

		GPU_CHECK(vkCreateDescriptorPool(mDevice.GetDevice(), &Info, NULL, &pDescPool->m_DescPool), "vkCreateDescriptorPool");
	}

	void AddDescBinding(VkDescriptorSetLayoutBinding mBind, vkDescriptorSet* pDescSet)
	{
		pDescSet->m_Bindings.push_back(mBind);
	}

	void GPU::AddDescBindings(std::span<const VkDescriptorSetLayoutBinding> mBinds, vkDescriptorSet* pDescSet)
	{
		for (auto& Bind : mBinds)
		{
			AddDescBinding(Bind, pDescSet);
		}
	}

	void AllocateDescSet(const vkDevice& mDevice, const vkDescriptorPool& mDescPool, VkDescriptorSetLayout mDescLayout, vkDescriptorSet* pDescSet)
	{
		AllocateDescSets(mDevice, mDescPool, std::span{ &mDescLayout, 1 }, std::span{ pDescSet, 1 });
	}

	void AllocateDescSets(const vkDevice& mDevice, const vkDescriptorPool& mDescPool, std::span<const VkDescriptorSetLayout> mDescLayouts, std::span<vkDescriptorSet> mDescSets)
	{
		VkDescriptorSetAllocateInfo Info = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = mDescPool.GetDescPool(),
			.descriptorSetCount = (uint32_t)mDescSets.size(),
			.pSetLayouts = mDescLayouts.data()
		};

		std::vector<VkDescriptorSet> DescSets(mDescSets.size());

		GPU_CHECK(vkAllocateDescriptorSets(mDevice.GetDevice(), &Info, DescSets.data()), "vkAllocateDescriptorSets");

		for (uint32_t i = 0; i < DescSets.size(); i++)
		{
			mDescSets[i].m_DescSet = DescSets[i];
		}
	}
}