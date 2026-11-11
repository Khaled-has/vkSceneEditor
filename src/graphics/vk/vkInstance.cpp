#include "vkInstance.h"
#include "vkLog.h"

namespace GPU
{
	void CreateInstance(const vkInstanceCreateInfo& mInfo, vkInstance* pInstance)
	{
		pInstance->m_ApiVersion = mInfo.mApiVersion;

		VkApplicationInfo appInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = mInfo.pAppName,
			.applicationVersion = mInfo.mAppVersion,
			.pEngineName = mInfo.pEngineName,
			.engineVersion = mInfo.mEngineVersion,
			.apiVersion = mInfo.mApiVersion
		};

		VkInstanceCreateInfo instInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
			.pApplicationInfo = &appInfo,
			.enabledLayerCount = (uint32_t)mInfo.mLayers.size(),
			.ppEnabledLayerNames = mInfo.mLayers.data(),
			.enabledExtensionCount = (uint32_t)mInfo.mExtensions.size(),
			.ppEnabledExtensionNames = mInfo.mExtensions.data()
		};

		// # Create the instance
		GPU_CHECK(vkCreateInstance(&instInfo, NULL, &pInstance->m_Instance), "vk: vkInstance create error");

		// # volk load instance
		volkLoadInstance(pInstance->GetInstance());
	}
}