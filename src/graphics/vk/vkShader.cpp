#include "vkShader.h"
#include "vkLog.h"

#include <fstream>

namespace GPU
{
	static std::string ReadFileString(const std::string& mPath)
	{
		std::ifstream file(mPath, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			GPU_LOG_ERROR("vkGPU: Failed to open the shader file: {0}", mPath);
		}

		size_t pSize = file.tellg();
		std::string buffer(pSize, '\0');

		file.seekg(0);
		file.read(buffer.data(), pSize);
		file.close();

		return buffer;
	}

#ifdef VKGPU_SHADERC_LOADER

	std::vector<uint32_t> CompileShader_shaderc(std::string_view mShaderCode, const vkShaderLoadInfo_shaderc& mInfo)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		// # Set optimization levels
		for (auto s : mInfo.mOptimizationLevels)
		{
			options.SetOptimizationLevel(s);
		}

		// # Compile the GLSL shader
		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
			mShaderCode.data(),
			mInfo.mShaderKind,
			mInfo.pName,
			options
		);

		// # Check the compilation status and print the shader errors
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			GPU_LOG_ERROR("vkGPU: vkShader error: {0}", result.GetErrorMessage());
		}

		return { result.cbegin(), result.cend() };
	}

	void LoadShader_shaderc(const vkDevice& mDevice, const char* pFilename, const vkShaderLoadInfo_shaderc& mInfo, vkShader* pShader)
	{
		// # Read the shader code from the file
		std::string_view ShaderCode = ReadFileString(pFilename);

		// # Send it to the read function
		ReadShader_shaderc(mDevice, ShaderCode, mInfo, pShader);
	}

	void ReadShader_shaderc(const vkDevice& mDevice, std::string_view mShaderCode, const vkShaderLoadInfo_shaderc& mInfo, vkShader* pShader)
	{
		// # Compile the shader code
		const std::vector<uint32_t> Spir_vCode = CompileShader_shaderc(mShaderCode.data(), mInfo);

		// # Create vulkan module
		VkShaderModuleCreateInfo Info = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = (size_t)(Spir_vCode.size() * sizeof(uint32_t)),
			.pCode = Spir_vCode.data()
		};

		GPU_CHECK(vkCreateShaderModule(mDevice.GetDevice(), &Info, NULL, &pShader->m_ShaderModule), "vkCreateShaderModule");
	}

#endif

#ifdef VKGPU_DXC_LOADER

	// # dxc components
	bool _DxcInited = false;
	ComPtr<IDxcUtils> _Utils;
	ComPtr<IDxcCompiler3> _Compiler;

	void SetDxcContext(ComPtr<IDxcUtils> cUtils, ComPtr<IDxcCompiler3> cCompiler)
	{
		_Utils = cUtils;
		_Compiler = cCompiler;
		_DxcInited = true;
	}

	// # Initialize function
	void InitDXC()
	{
		HRESULT Hres;

		// # Create the library
		//Hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_Library));
		//if (FAILED(Hres))
		//{
		//	GPU_LOG_ERROR("vkGPU: vkShader error in dxc: Could not init DXC Library {0}", Hres);
		//	exit(1);
		//}
		
		// # Create the utility
		Hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&_Utils));
		if (FAILED(Hres))
		{
			GPU_LOG_ERROR("vkGPU: vkShader error in dxc: Could not init DXC Utils");
			exit(1);
		}

		// # Create the compiler
		Hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_Compiler));
		if (FAILED(Hres))
		{
			GPU_LOG_ERROR("vkGPU: vkShader error in dxc: Could not init DXC Compiler");
			exit(1);
		}


		_DxcInited = true;
	}


	void CompileShader_dxc(const std::string& mCode, const vkShaderLoadInfo_dxc& mInfo, void*& pData, size_t* pSize)
	{
		HRESULT Hres;

		// Compile shader
		DxcBuffer Buffer{};
		Buffer.Ptr = mCode.data();
		Buffer.Size = SIZE_T(mCode.size() * sizeof(char));
		Buffer.Encoding = DXC_CP_UTF8;

		ComPtr<IDxcResult> Result{ nullptr };
		Hres = _Compiler->Compile(
			&Buffer,
			mInfo.mArguments.data(),
			(uint32_t)mInfo.mArguments.size(),
			nullptr,
			IID_PPV_ARGS(&Result)
		);

		if (SUCCEEDED(Hres))
		{
			Result->GetStatus(&Hres);
		}

		// # Output error if compilation failed
		if (FAILED(Hres) && (Result))
		{
			ComPtr<IDxcBlobEncoding> errorBlob;
			Hres = Result->GetErrorBuffer(&errorBlob);

			if (SUCCEEDED(Hres) && errorBlob)
			{
				GPU_LOG_ERROR("vkGPU: vkShader compilation failed :\n\n {0}", (const char*)errorBlob->GetBufferPointer());
				exit(1);
			}
		}

		// # Get compilation result
		ComPtr<IDxcBlob> Code;
		Result->GetResult(&Code);

		pData = Code->GetBufferPointer();
		*pSize = Code->GetBufferSize();
	}

	void LoadShader_dxc(const vkDevice& mDevice, const char* pFilename, const vkShaderLoadInfo_dxc& mInfo, vkShader* pShader)
	{
		std::string ShaderCode = ReadFileString(pFilename);

		ReadShader_dxc(mDevice, ShaderCode.data(), mInfo, pShader);
	}

	void ReadShader_dxc(const vkDevice& mDevice, const std::string& mCode, const vkShaderLoadInfo_dxc& mInfo, vkShader* pShader)
	{
		if (!_DxcInited)
		{
			GPU_LOG_ERROR("vkGPU: vkShader error: You can't call some of DXC function before you call GPU::SetDxcContext()");
			exit(1);
		}

		void* pData;
		size_t Size;
		CompileShader_dxc(mCode, mInfo, pData, &Size);


		// # Create vulkan module
		VkShaderModuleCreateInfo Info = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = Size,
			.pCode = (uint32_t*)pData
		};

		GPU_CHECK(vkCreateShaderModule(mDevice.GetDevice(), &Info, NULL, &pShader->m_ShaderModule), "vkCreateShaderModule");
	}

#endif
}