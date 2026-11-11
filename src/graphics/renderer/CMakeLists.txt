
add_library(vkRenderer)

target_sources(vkRenderer 
	PRIVATE
		# Renderer components
		"Renderer.cpp" "Camera.cpp" 
		"vkRenderer.cpp" "vkRenderTarget.cpp"

		# Passes
		"Passes/RenderGraph.cpp" "Passes/GeometryPass.cpp"

		# ImGui
		"ImGui/ImGuiRenderer.cpp" "ImGui/ImGuiAsset.cpp" "ImGui/ImGuiImage.cpp"

		# Compiler
		"Compilers/DxCompiler.cpp"

	PUBLIC 
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		# Renderer
		"Renderer.h" "Camera.h"
		# ImGui
		"ImGui/ImGuiImage.h" "ImGui/ImGuiAsset.h"
)

target_link_libraries(vkRenderer 
	PRIVATE
		vkGPU
		stbi

	PUBLIC
		ImGui
		codicon
		glm
)