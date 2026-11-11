#include "vkDevice.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

#include "vkLog.h"

namespace GPU
{

	void CreateDevice(const vkDeviceCreateInfo& mInfo, vkDevice* pDevice)
	{
		pDevice->m_pTargetPhysDevice = mInfo.pPhysDevice;
		
		// # Queues info
		auto prop = pDevice->m_pTargetPhysDevice->GetInfo();
		auto& Queues = pDevice->m_DeviceQueueInfo;

		uint32_t QFCount = prop.mFamilyProps.size();
		const std::vector<VkQueueFamilyProperties> QFamilyProps = prop.mFamilyProps;

		std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos;

		uint32_t numProperties = 0;

		for (uint32_t i = 0; i < QFCount; i++)
		{
			const VkQueueFamilyProperties& curProps = QFamilyProps[i];

			bool validQueue = false;

			// # 1: Graphics queue
			if (((curProps.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) && Queues.mGraphics == -1)
			{
				Queues.mGraphics = i;
				validQueue = true;
			}

			// # 2: Compute queue
			if (
				((curProps.queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT) && Queues.mCompute == -1 &&
				Queues.mGraphics != i
				)
			{
				Queues.mCompute = i;
				validQueue = true;
			}

			// # 3: Transfer queue
			if (
				((curProps.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) && Queues.mTransfer == -1 &&
				Queues.mGraphics != i && Queues.mCompute != i
				)
			{
				Queues.mTransfer = i;
				validQueue = true;
			}

			if (!validQueue)
			{
				continue;
			}

			// # Add the new queue
			VkDeviceQueueCreateInfo qCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = i,
				.queueCount = curProps.queueCount,
			};

			queueFamilyInfos.push_back(qCreateInfo);
			numProperties += curProps.queueCount;
		}

		GPU_ASSERT(pDevice->m_DeviceQueueInfo.mGraphics != -1, "Missing Vulkan queue graphics");

		if (Queues.mCompute == -1) Queues.mCompute = Queues.mGraphics;
		if (Queues.mTransfer == -1) Queues.mTransfer = Queues.mGraphics;
		
		std::vector<float> queueProperties(numProperties);
		float* currentPriority = queueProperties.data();

		for (uint32_t i = 0; i < queueFamilyInfos.size(); i++)
		{
			queueFamilyInfos[i].pQueuePriorities = currentPriority;

			for (uint32_t j = 0; j < QFamilyProps[queueFamilyInfos[i].queueFamilyIndex].queueCount; j++)
			{
				*currentPriority++ = 1.0f;
			}
		}

		// # Device features
		if (mInfo.mEnableDynamicRendering && !(mInfo.pPhysDevice->GetInfo().mIsDynamicRenderingSupported))
		{
			GPU_LOG_ERROR("vkGPU: Dynamic rendering is not supported at this physical device");
			exit(1);
		}

		VkPhysicalDeviceDynamicRenderingFeaturesKHR DynamicRenderingFeature = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
			.dynamicRendering = mInfo.mEnableDynamicRendering ? VK_TRUE : VK_FALSE,
		};
		
		// # Device creation
		VkDeviceCreateInfo DeviceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &DynamicRenderingFeature,
			.queueCreateInfoCount = (uint32_t)queueFamilyInfos.size(),
			.pQueueCreateInfos = queueFamilyInfos.data(),
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = NULL,
			.enabledExtensionCount = (uint32_t)mInfo.mExtensions.size(),
			.ppEnabledExtensionNames = mInfo.mExtensions.data(),
			.pEnabledFeatures = &mInfo.pPhysDevice->GetInfo().mFeatures,
		};
		
		GPU_CHECK(vkCreateDevice(pDevice->m_pTargetPhysDevice->GetPhysDevice(), &DeviceCreateInfo, NULL, &pDevice->m_Device), "vkCreateDevice");
		
		// # volk load device
		volkLoadDevice(pDevice->GetDevice());
	}
}