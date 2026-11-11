#pragma once

#include "vkConfig.h"

namespace GPU
{
	struct vkInstanceCreateInfo
	{
		const char* pAppName;
		const char* pEngineName;

		uint32_t mAppVersion = VK_MAKE_VERSION(1, 0, 0);
		uint32_t mEngineVersion = VK_MAKE_VERSION(1, 0, 0);

		uint32_t mApiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		std::span<const char*> mLayers;
		std::span<const char*> mExtensions;
	};

	class VKGPU_API vkInstance
	{
		friend void CreateInstance(const vkInstanceCreateInfo& mInfo, vkInstance* pInstance);
	public:
		vkInstance() {}

		inline VkInstance GetInstance() const { return m_Instance; }

		inline uint32_t GetApiVersion() const { return m_ApiVersion; }

	private:
		VkInstance m_Instance;
		uint32_t m_ApiVersion;
	};

	VKGPU_API void CreateInstance(const vkInstanceCreateInfo& mInfo, vkInstance* pInstance);
}