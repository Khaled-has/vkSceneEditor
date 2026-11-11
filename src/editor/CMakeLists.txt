
add_library(vkEditor)

target_sources(vkEditor
	PRIVATE
		"Editor.cpp" "TitleBar.cpp" "Hierarchy/Hierarchy.cpp"
		"Hierarchy/Inspector.cpp" "Viewport.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Editor.h"
)

target_link_libraries(vkEditor
	PRIVATE
		vkRenderer
		ImGui
		editorLog
	PUBLIC
		vkScene
)