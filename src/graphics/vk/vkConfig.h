#pragma once

#include <iostream>
#include <cstdint>
#include <stdint.h>
#include <vector>
#include <functional>
#include <span>

// # volk - first include
#include <volk.h>
// # vma - secound include
#include <vk_mem_alloc.h>

#include <vulkan/vk_enum_string_helper.h>

// # vkGPU API
#ifndef VKGPU_API
	#define VKGPU_API
#endif