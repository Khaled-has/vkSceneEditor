
add_library(vkScene)

target_sources(vkScene
	PRIVATE
		"Scene.cpp" "Entity.cpp"
	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
		"Scene.h" "Entity.h" "Components.h"
)

target_link_libraries(vkScene 
	PUBLIC 
		EnTT
		glm
)