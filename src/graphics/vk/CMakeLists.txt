
add_library(vkGPU)

target_sources(vkGPU
	PRIVATE
		 "vkInstance.cpp" "vkPhysDevice.cpp" "vkDevice.cpp" "vkSwapChain.cpp"
		 "vkQueue.cpp" "vkCmdBuf.cpp" "vkSync.cpp" "vkPipeline.cpp" "vkRenderPass.cpp"
		 "vkDescriptorSet.cpp" "vkShader.cpp" "vkBuffer.cpp" "vkTexture.cpp" 
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"vkGPU.h"
)

target_link_libraries(vkGPU 
	PUBLIC
	volk
	vma

	dxc

	editorLog
)