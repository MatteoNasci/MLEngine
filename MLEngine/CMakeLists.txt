cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(MLEngine VERSION 1.0 DESCRIPTION "Simple game engine made by Matteo Lorenzo Nasci")

# OpenGL
#find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
    message('OPENGL_FOUND-is-true')
else()
    message('OPENGL_FOUND-is-false')
endif()

set(MLEngine_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(MLEngine_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(MLEngine_EXT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/)
set(GLFW_ROOT ${MLEngine_EXT_ROOT}/Glfw/)
set(GLFW_INCLUDE_ROOT ${GLFW_ROOT})
set(GLAD_KHR_ROOT ${MLEngine_EXT_ROOT}/Glad/)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GLFW_LIBS ${GLFW_ROOT}/lib/debug/glfw3.lib)
    set(GLAD_KHR_INCLUDE_ROOT ${GLAD_KHR_ROOT}/debug/include/)
    set(GLAD_KHR_SOURCE_ROOT ${GLAD_KHR_ROOT}/debug/src/)
else()
    set(GLFW_LIBS ${GLFW_ROOT}/lib/release/glfw3.lib)
    set(GLAD_KHR_INCLUDE_ROOT ${GLAD_KHR_ROOT}/release/include/)
    set(GLAD_KHR_SOURCE_ROOT ${GLAD_KHR_ROOT}/release/src/) 
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mlengineconfig.h.in ${MLEngine_INCLUDE_ROOT}/mlengineconfig.h)

add_library(GLAD STATIC ${GLAD_KHR_SOURCE_ROOT}glad.c)
target_include_directories(GLAD PUBLIC "${GLAD_KHR_INCLUDE_ROOT}")
target_link_libraries(GLAD PUBLIC opengl32)

set(MLEngine_LIBS ${GLFW_LIBS} GLAD)

# Add source files
#file(GLOB_RECURSE SOURCE_FILES 
#	${CMAKE_SOURCE_DIR}/src/*.c
#	${CMAKE_SOURCE_DIR}/src/*.cpp)
	
# Add header files
#file(GLOB_RECURSE HEADER_FILES 
#	${CMAKE_SOURCE_DIR}/src/*.h
#	${CMAKE_SOURCE_DIR}/src/*.hpp)

add_library(MLEngine SHARED 
    ${MLEngine_SOURCE_ROOT}test.cpp
    )

target_link_libraries(MLEngine PRIVATE ${MLEngine_LIBS})
target_compile_definitions(MLEngine PRIVATE MLENGINE_LIBRARY)
target_include_directories(MLEngine PRIVATE "${MLEngine_INCLUDE_ROOT}" "${GLFW_INCLUDE_ROOT}" "${GLAD_KHR_INCLUDE_ROOT}")
