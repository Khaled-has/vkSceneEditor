#include "vkPhysDevice.h"
#include "vkPhysDevice.h"
#include "vkLog.h"

namespace GPU
{

	VkFormat FindSupportedFormat(VkPhysicalDevice Device, const std::vector<VkFormat>& Candidates,
		VkImageTiling Tiling, VkFormatFeatureFlags Features)
	{
		for (int i = 0; i < Candidates.size(); i++)
		{
			VkFormat Format = Candidates[i];
			VkFormatProperties Props;
			vkGetPhysicalDeviceFormatProperties(Device, Format, &Props);

			if ((Tiling == VK_IMAGE_TILING_LINEAR) &&
				(Props.linearTilingFeatures & Features) == Features)
			{
				return Format;
			}
			else if ((Tiling == VK_IMAGE_TILING_OPTIMAL) &&
				(Props.optimalTilingFeatures & Features) == Features)
			{
				return Format;
			}
		}

		GPU_LOG_ERROR("vkGPU: Failed to find supported format!");
		exit(1);
	}

	VkFormat FindDepthFormat(VkPhysicalDevice Device)
	{
		std::vector<VkFormat> Candidates = {
			VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT
		};

		VkFormat DepthFormat = FindSupportedFormat(
			Device, Candidates, VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);

		return DepthFormat;
	}

    uint32_t GetPhysDevicesCount(const vkInstance& mInstance)
    {
        uint32_t res = 0;
        vkEnumeratePhysicalDevices(mInstance.GetInstance(), &res, NULL);
        return res;
    }

    void EnumeratePhysDevices(const vkInstance& mInstance, VkSurfaceKHR mSurface, std::span<vkPhysDevice> mPhysDevices)
    {
        // # Check
		uint32_t dCount = GetPhysDevicesCount(mInstance);
        if (mPhysDevices.size() != dCount)
        {
            GPU_LOG_ERROR("vkGPU: You can't set number of PhysDevices large than the GetPhysDevicesCount() func get you in the EnumeratePhysDevices() func");
            exit(1);
        }

		// # Enumerate the physical devices
		std::vector<VkPhysicalDevice> devices(dCount);
		vkEnumeratePhysicalDevices(mInstance.GetInstance(), &dCount, devices.data());

        // # Create the physical devices
        for (uint32_t i = 0; i < dCount; i++)
        {
			VkPhysicalDevice PhysDev = devices[i];

			mPhysDevices[i].m_PhysDevice = PhysDev;
			auto& Info = mPhysDevices[i].m_Info;

			// # Device properties
			vkGetPhysicalDeviceProperties(PhysDev, &Info.mDevProps);

			// # Device extensions
			uint32_t extenCount = 0;
			vkEnumerateDeviceExtensionProperties(PhysDev, NULL, &extenCount, NULL);
			Info.mExtensions.resize(extenCount);
			vkEnumerateDeviceExtensionProperties(PhysDev, NULL, &extenCount, Info.mExtensions.data());

			// # Device queue families properties
			uint32_t NumQFamilies = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &NumQFamilies, NULL);
			GPU_ASSERT(NumQFamilies >= 1, "Queue families = 0");

			Info.mFamilyProps.resize(NumQFamilies);
			Info.mSupportsPresent.resize(NumQFamilies);

			vkGetPhysicalDeviceQueueFamilyProperties(PhysDev, &NumQFamilies, Info.mFamilyProps.data());

			for (uint32_t q = 0; q < NumQFamilies; q++)
			{
				const VkQueueFamilyProperties& QFamilyProp = Info.mFamilyProps[q];
			}

			// # Device surface formats
			uint32_t NumFormats = 0;
			GPU_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysDev, mSurface, &NumFormats, NULL), "vkGetPhysicalDeviceSurfaceFormats");
			GPU_ASSERT(NumFormats > 0, "Num surface formats = 0");

			Info.mSurfaceFormats.resize(NumFormats);

			GPU_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysDev, mSurface, &NumFormats, Info.mSurfaceFormats.data()), "vkGetPhysicalDeviceSurfaceFormatsKHR");

			// # Device surface capabilities
			GPU_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysDev, mSurface, &(Info.mSurfaceCaps)), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

			// # Device present modes
			uint32_t NumPresentModes = 0;
			GPU_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysDev, mSurface, &NumPresentModes, NULL), "vkGetPhysicalDeviceSurfacePresentModesKHR");
			GPU_ASSERT(NumPresentModes > 0, "Num present modes = 0");

			Info.mPresentModes.resize(NumPresentModes);

			GPU_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysDev, mSurface, &NumPresentModes, Info.mPresentModes.data()), "vkGetPhysicalDeviceSurfacePresentModesKHR");

			// # Device memory properties
			vkGetPhysicalDeviceMemoryProperties(PhysDev, &(Info.mMemProps));
			
			// # Device features
			//Info.mFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

			vkGetPhysicalDeviceFeatures(PhysDev, &(Info.mFeatures));
			//vkGetPhysicalDeviceFeatures2(PhysDev, &(Info.mFeatures2));

			// # Device depth format
			Info.mDepthFormat = FindDepthFormat(PhysDev);

			// # Check supporting dynamic rendering
			Info.mIsDynamicRenderingSupported = IsExtensionSupported(
				mPhysDevices[i], 
				VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
			);
        }
    }

	bool GPU::IsExtensionSupported(const vkPhysDevice& mPhysDevice, const char* pExtension)
	{
		for (const auto& e : mPhysDevice.GetInfo().mExtensions)
		{
			if (std::string(e.extensionName) == std::string(pExtension))
				return true;
		}
		return false;
	}
}