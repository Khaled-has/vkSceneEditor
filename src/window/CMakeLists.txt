
add_library(vkWindow)

target_sources(vkWindow
	PRIVATE
		"window.cpp" 
		"windows/windowsWindow.cpp" "windows/windowsCustumTitleBar.cpp"

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"window.h"
)

target_link_libraries(vkWindow
	PRIVATE
		vkRenderer
		SDL3::SDL3
		volk
		ImGui
		editorLog
)

if(WIN32)

	target_link_libraries(vkWindow
		PRIVATE
			dwmapi
			gdi32
			user32
			kernel32
	)

endif()