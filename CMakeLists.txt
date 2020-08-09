cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(MLEngineTest VERSION 1.0)

set(MLEngine_INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/MLEngine/include/)
set(MLEngineTest_INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/Test/include/)
set(MLEngineTest_SOURCE_ROOT ${CMAKE_SOURCE_DIR}/Test/src/)

add_subdirectory(MLEngine)

# add the executable
add_executable(MLEngineTest ${MLEngineTest_SOURCE_ROOT}main.cpp)

target_include_directories(MLEngineTest PUBLIC "${MLEngineTest_INCLUDE_ROOT}" "${MLEngine_INCLUDE_ROOT}")
target_link_libraries(MLEngineTest PUBLIC MLEngine GLAD opengl32)


# Copy dlls
if(WIN32)
    #add_custom_command(
    #    TARGET MLEngine POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E remove_directory
    #    "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/release>"
    #    "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/debug>"
    #)

    #add_custom_command(
    #    TARGET MLEngine POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_directory
    #    $<TARGET_FILE_DIR:holey_scanner>
    #    "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/lib/release>"
    #    "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/lib/debug>"
    #)

	#add_custom_command(TARGET MLEngine POST_BUILD
	#	COMMAND ${CMAKE_COMMAND} -E copy_directory
    #    "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/Release/"
    #    "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/Debug/"
	#	$<TARGET_FILE_DIR:MLEngine>)

    add_custom_command(TARGET MLEngineTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/build/MLEngine/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:MLEngineTest>)
endif()