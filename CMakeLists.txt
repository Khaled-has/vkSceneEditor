cmake_minimum_required(VERSION 3.26)

project(vkSceneEditor)

if(MSVC) 
	add_compile_options("/utf-8")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(WIN32) # SDL3 add the path from extern folder for WIN32

	list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/SDL3")
	find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3)

endif()

add_executable(${PROJECT_NAME} "src/main.cpp" "src/Application.cpp")

target_link_libraries(${PROJECT_NAME} 
	PRIVATE
		vkRenderer
		vkWindow

		vkEditor
)

if (WIN32) # Dlls for windows platform

	# SDL3 dll
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/SDL3/lib/x64/SDL3.dll"
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)

	# dxc dll
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/extern/dxc/bin/x64/dxcompiler.dll"
			$<TARGET_FILE_DIR:${PROJECT_NAME}>
	)

endif()

# Copy the shaders folder
set(SHADERS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADERS_DST_DIR "${CMAKE_BINARY_DIR}/shaders")

add_custom_target(CopyShaders ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${SHADERS_SOURCE_DIR}"
		"${SHADERS_DST_DIR}"
	COMMENT "Copying shaders to output directory..."
)
# Copy the resources folder
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
set(RESOURCES_DST_DIR "${CMAKE_BINARY_DIR}/res")

add_custom_target(CopyResources ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${RESOURCES_DIR}"
		"${RESOURCES_DST_DIR}"
	COMMENT "Copying resources to output directory..."
)

add_dependencies(${PROJECT_NAME} CopyShaders CopyResources)

add_subdirectory("src/window")				# window
add_subdirectory("src/graphics/renderer")	# renderer
add_subdirectory("src/graphics/vk")			# vkGPU
add_subdirectory("src/scene")				# scene
add_subdirectory("src/log")					# log

add_subdirectory("src/editor")				# editor

add_subdirectory("extern")					# libraries