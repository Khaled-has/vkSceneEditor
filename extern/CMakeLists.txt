
# volk
add_library(volk)

target_sources(volk
	PRIVATE
		"volk/volk.c"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/volk"
	FILES
		"volk/volk.h"
)

target_include_directories(volk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include")

# vma
add_library(vma INTERFACE)

target_include_directories(vma 
	INTERFACE 
		"${CMAKE_CURRENT_SOURCE_DIR}/vulkan/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/vma"
)

set_target_properties(vma PROPERTIES LINKER_LANGUAGE CXX)

# glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/glm")

# shaderc
add_library(shaderc INTERFACE)
target_include_directories(shaderc INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/shaderc")

# dxc
add_library(dxc INTERFACE)

target_include_directories(dxc INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/dxc/include")
target_link_libraries(dxc INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/dxc/lib/x64/dxcompiler.lib")

# ImGui
add_library(ImGui)

target_sources(ImGui
	PRIVATE
		"ImGui/imgui.cpp" "ImGui/imgui_demo.cpp" "ImGui/imgui_draw.cpp"
		"ImGui/imgui_impl_vulkan.cpp" "ImGui/imgui_impl_sdl3.cpp" "ImGui/imgui_tables.cpp" "ImGui/imgui_widgets.cpp" 
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ImGui"
	FILES
		"ImGui/imgui.h" "ImGui/imgui_impl_vulkan.h" "ImGui/imgui_impl_sdl3.h"
)

target_include_directories(ImGui
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/SDL3/include"
)

target_link_libraries(ImGui PUBLIC volk)

# stbi
add_library(stbi)

target_sources(stbi
	PRIVATE
		"stb/stb_image.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/stb"
	FILES
		"stb/stb_image.h"
)

# codicon
add_library(codicon INTERFACE)

target_include_directories(codicon INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/codicon")

# EnTT
add_library(EnTT INTERFACE)

target_include_directories(EnTT INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/entt")