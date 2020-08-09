cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(MLEngine VERSION 1.0 DESCRIPTION "Simple game engine made by Matteo Lorenzo Nasci")

set(MLEngine_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(MLEngine_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(MLEngine_EXT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/)
set(GLFW_ROOT ${MLEngine_EXT_ROOT}/Glfw/)
set(GLFW_INCLUDE_ROOT ${GLFW_ROOT}/include/)

add_subdirectory(${GLFW_ROOT})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

else()

endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mlengineconfig.h.in ${MLEngine_INCLUDE_ROOT}/mlengineconfig.h)

set(MLEngine_LIBS glfw opengl32)

# Add source files
file(GLOB_RECURSE SOURCE_FILES 
	${MLEngine_SOURCE_ROOT}/*.c
	${MLEngine_SOURCE_ROOT}/*.cpp)

add_library(MLEngine SHARED 
    ${SOURCE_FILES}
    )

target_link_libraries(MLEngine PRIVATE ${MLEngine_LIBS})
target_compile_definitions(MLEngine PRIVATE MLENGINE_LIBRARY)
target_include_directories(MLEngine PRIVATE "${MLEngine_INCLUDE_ROOT}" "${GLFW_INCLUDE_ROOT}")
