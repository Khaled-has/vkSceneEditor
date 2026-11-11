#pragma once

#include "vkConfig.h"
#include "vkInstance.h"

namespace GPU
{
	class VKGPU_API vkPhysDevice
	{
		friend void EnumeratePhysDevices(const vkInstance& mInstance, VkSurfaceKHR mSurface, std::span<vkPhysDevice> mPhysDevices);
	public:
		vkPhysDevice() {}

		const VkPhysicalDevice& GetPhysDevice() const { return m_PhysDevice; }

		inline const auto& GetInfo()			const { return m_Info; }

	private:
		VkPhysicalDevice m_PhysDevice = VK_NULL_HANDLE;

		struct {
			VkPhysicalDevice mPhysDev;
			VkPhysicalDeviceProperties mDevProps;
			std::vector<VkQueueFamilyProperties> mFamilyProps;
			std::vector<VkBool32> mSupportsPresent;
			std::vector<VkSurfaceFormatKHR> mSurfaceFormats;
			VkSurfaceCapabilitiesKHR mSurfaceCaps;
			VkPhysicalDeviceMemoryProperties mMemProps;
			std::vector<VkPresentModeKHR> mPresentModes;
			VkPhysicalDeviceFeatures mFeatures;
			VkPhysicalDeviceFeatures2 mFeatures2;
			bool mIsDynamicRenderingSupported;
			VkFormat mDepthFormat;
			std::vector<VkExtensionProperties> mExtensions;
		} m_Info;
	};

	VKGPU_API uint32_t GetPhysDevicesCount(const vkInstance& mInstance);
	VKGPU_API void EnumeratePhysDevices(const vkInstance& mInstance, VkSurfaceKHR mSurface, std::span<vkPhysDevice> mPhysDevices);

	VKGPU_API bool IsExtensionSupported(const vkPhysDevice& mPhysDevice, const char* pExtension);
}