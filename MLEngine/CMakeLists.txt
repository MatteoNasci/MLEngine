cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set(VCPKG_TARGET_TRIPLET "x64-windows-static")

# set the project name
project(MLEngine VERSION 1.0 DESCRIPTION "Simple game engine made by Matteo Lorenzo Nasci")

if (NOT "$ENV{VULKAN_SDK}" STREQUAL "")
    set(VULKAN_SDK_ROOT "$ENV{VULKAN_SDK}" CACHE INTERNAL "Copied from environment variable")
    message("Found environment path for Vulkan sdk!")
endif()

set(MLEngine_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(MLEngine_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(MLEngine_EXT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/)
set(GLFW_ROOT ${MLEngine_EXT_ROOT}/Glfw/)
set(GLFW_INCLUDE_ROOT ${GLFW_ROOT}/include/)
set(VULKAN_INCLUDE_ROOT ${VULKAN_SDK_ROOT}/Include/)
set(GML_INCLUDE_ROOT ${MLEngine_EXT_ROOT}/glm-0.9.9.8/glm/glm)
set(GLFW_LIBS_ROOT ${GLFW_ROOT}/build/src/)

find_package(Vulkan REQUIRED)

set(BUILD_MLENGINE_STATIC TRUE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mlengineconfig.h.in ${MLEngine_INCLUDE_ROOT}/mlengineconfig.h)

set(MLEngine_LIBS opengl32)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${MLEngine_SOURCE_ROOT}/*.c
	${MLEngine_SOURCE_ROOT}/*.cpp)

if(BUILD_MLENGINE_STATIC)
add_library(MLEngine STATIC 
    ${SOURCE_FILES}
    )
target_compile_definitions(MLEngine PUBLIC MLENGINE_STATIC)
else()
add_library(MLEngine SHARED 
    ${SOURCE_FILES}
    )
endif()

target_link_libraries(MLEngine PRIVATE general ${MLEngine_LIBS} ${VULKAN_SDK_ROOT}/Lib/vulkan-1.lib)
target_link_libraries(MLEngine PRIVATE debug ${GLFW_LIBS_ROOT}/Debug/glfw3.lib)
target_link_libraries(MLEngine PRIVATE optimized ${GLFW_LIBS_ROOT}/Release/glfw3.lib)

target_compile_definitions(MLEngine PRIVATE MLENGINE_LIBRARY)
target_include_directories(MLEngine PRIVATE "${GLFW_INCLUDE_ROOT}" "${VULKAN_INCLUDE_ROOT}")
target_include_directories(MLEngine PUBLIC "${GML_INCLUDE_ROOT}" "${MLEngine_INCLUDE_ROOT}")

#Copy libraries
if(WIN32)
    add_custom_command(TARGET MLEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/build/MLEngine/$<CONFIGURATION>"
        "${CMAKE_SOURCE_DIR}/build/MLEngineLibs/$<CONFIGURATION>")
endif()
