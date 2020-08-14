cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# set the project name
project(MLEngineTest VERSION 1.0)

set(MLEngine_INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/MLEngine/include/)
set(MLEngineTest_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(MLEngineTest_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(MLEngineTest_REMOVE_CONSOLE FALSE)

file(GLOB_RECURSE SOURCE_FILES 
	${MLEngineTest_SOURCE_ROOT}/*.c
	${MLEngineTest_SOURCE_ROOT}/*.cpp)

# add the executable
add_executable(MLEngineTest ${SOURCE_FILES})

target_include_directories(MLEngineTest PUBLIC "${MLEngineTest_INCLUDE_ROOT}" "${MLEngine_INCLUDE_ROOT}")
target_link_libraries(MLEngineTest PUBLIC MLEngine)

#removes the console from the application in windows
if(WIN32)
    if(MLEngineTest_REMOVE_CONSOLE)
        set_target_properties(MLEngineTest PROPERTIES 
            LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
    endif()
endif()

#Copy libraries
if(WIN32)
    add_custom_command(TARGET MLEngineTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/build/MLEngine/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:MLEngineTest>)
endif()
