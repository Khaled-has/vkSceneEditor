
add_library(editorLog)

target_sources(editorLog
	PRIVATE
		"log.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"log.h"
)