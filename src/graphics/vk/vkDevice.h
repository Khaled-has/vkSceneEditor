#ifndef VKDEVICE_H
#define VKDEVICE_H

#include "vkPhysDevice.h"

namespace GPU
{
	struct vkDeviceCreateInfo
	{
		const vkPhysDevice* pPhysDevice;

		VkPhysicalDeviceFeatures mFeatures;
		VkPhysicalDeviceFeatures2 mFeatures2;

		bool mEnableDynamicRendering = /* Default value = */ false;

		std::vector<const char*> mExtensions; /* = Example EXT  {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME,
			VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
		}; */
	};

	class VKGPU_API vkDevice
	{
		friend void CreateDevice(const vkDeviceCreateInfo& mInfo, vkDevice* pDevice);
	public:
		vkDevice() {}

		inline const VkDevice& GetDevice()				const { return m_Device; }
		inline const vkPhysDevice& GetPhysDevice()		const { return *m_pTargetPhysDevice; }
		inline const auto& GetDeviceQueueInfo()			const { return m_DeviceQueueInfo; }

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		const vkPhysDevice* m_pTargetPhysDevice;

		struct {
			int32_t mGraphics = -1;
			int32_t mCompute  = -1;
			int32_t mTransfer = -1;
		} m_DeviceQueueInfo;
	};

	VKGPU_API void CreateDevice(const vkDeviceCreateInfo& mInfo, vkDevice* pDevice);
}

#endif