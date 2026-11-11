#ifndef VKSHADER_H
#define VKSHADER_H

#include "vkConfig.h"
#include "vkDevice.h"

#if __has_include(<shaderc.hpp>)
#define VKGPU_SHADERC_LOADER
#include <shaderc.hpp>
#endif

#if __has_include(<dxcapi.h>)
#define VKGPU_DXC_LOADER

#ifdef WIN32
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#endif

#include <dxcapi.h>

#elif __has_include(<dxc/include/dxcapi.h>)
#define DXC_LOADER

#ifdef WIN32
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
#endif

#include <dxc/dxcapi.h>
#endif

namespace GPU
{
#ifdef VKGPU_SHADERC_LOADER

	struct vkShaderLoadInfo_shaderc
	{
		const char* pName;
		shaderc_shader_kind mShaderKind;
		std::span<shaderc_optimization_level> mOptimizationLevels;
	};

#endif

#ifdef VKGPU_DXC_LOADER

	struct vkShaderLoadInfo_dxc
	{
		std::span<LPCWSTR> mArguments;
	};

#endif

	class VKGPU_API vkShader
	{
#ifdef VKGPU_SHADERC_LOADER
		friend void LoadShader_shaderc(const vkDevice& mDevice, const char* pFilename, const vkShaderLoadInfo_shaderc& mInfo, vkShader* pShader);
		friend void ReadShader_shaderc(const vkDevice& mDevice, const char* pCode, const vkShaderLoadInfo_shaderc& mInfo, vkShader* pShader);
#endif

#ifdef VKGPU_DXC_LOADER
		friend void LoadShader_dxc(const vkDevice& mDevice, const char* pFilename, const vkShaderLoadInfo_dxc& mInfo, vkShader* pShader);
		friend void ReadShader_dxc(const vkDevice& mDevice, const std::string& mCode, const vkShaderLoadInfo_dxc& mInfo, vkShader* pShader);
#endif
	public:
		vkShader() {}
		
		inline const VkShaderModule& GetShaderModule() const { return m_ShaderModule; }
		
	private:
		VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
	};

#ifdef VKGPU_SHADERC_LOADER
	VKGPU_API void LoadShader_shaderc(const vkDevice& mDevice, const char* pFilename, const vkShaderLoadInfo_shaderc& mInfo, vkShader* pShader);
	VKGPU_API void ReadShader_shaderc(const vkDevice& mDevice, std::string_view mShaderCode, const vkShaderLoadInfo_shaderc& mInfo, vkShader* pShader);
#endif

#ifdef VKGPU_DXC_LOADER
	VKGPU_API void SetDxcContext(ComPtr<IDxcUtils> cUtils, ComPtr<IDxcCompiler3> cCompiler);

	VKGPU_API void LoadShader_dxc(const vkDevice& mDevice, const char* pFilename, const vkShaderLoadInfo_dxc& mInfo, vkShader* pShader);
	VKGPU_API void ReadShader_dxc(const vkDevice& mDevice, const std::string& mCode, const vkShaderLoadInfo_dxc& mInfo, vkShader* pShader);
#endif
}

#endif