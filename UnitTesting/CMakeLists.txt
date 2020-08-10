cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# set the project name
project(MLEngineUnitTesting VERSION 1.0)

set(UnitTest_EXT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/)
set(GoogleTest_Gmock_INCLUDE_ROOT ${UnitTest_EXT_ROOT}/GoogleTest/googlemock/include/)
set(GoogleTest_test_INCLUDE_ROOT ${UnitTest_EXT_ROOT}/GoogleTest/googletest/include/)
set(UnitTest_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(UnitTest_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_subdirectory(ext/GoogleTest)

set(UnitTest_LIBS MLEngine gmock)

file(GLOB_RECURSE SOURCE_FILES 
	${UnitTest_SOURCE_ROOT}/*.c
	${UnitTest_SOURCE_ROOT}/*.cpp)

add_executable(MLEngineUnitTesting ${SOURCE_FILES})

target_link_libraries(MLEngineUnitTesting PRIVATE ${UnitTest_LIBS})
target_include_directories(MLEngineUnitTesting PRIVATE "${UnitTest_INCLUDE_ROOT}" "${GoogleTest_test_INCLUDE_ROOT}" "${GoogleTest_Gmock_INCLUDE_ROOT}")

#Copy libraries
if(WIN32)
    add_custom_command(TARGET MLEngineUnitTesting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/build/MLEngine/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:MLEngineUnitTesting>)

    add_custom_command(TARGET MLEngineUnitTesting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/build/lib/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:MLEngineUnitTesting>)

    add_custom_command(TARGET MLEngineUnitTesting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/build/bin/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:MLEngineUnitTesting>)
endif()
