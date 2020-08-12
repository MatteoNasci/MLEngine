cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(MLEngineProject VERSION 1.0)

set(BUILD_ENGINE_PROJECT TRUE)
set(BUILD_TEST_PROJECT TRUE)
set(BUILD_ENGINE_UNITTEST_PROJECT TRUE)

if(BUILD_ENGINE_PROJECT)
add_subdirectory(MLEngine)
endif()

if(BUILD_TEST_PROJECT)
add_subdirectory(Test)
endif()

if(BUILD_ENGINE_UNITTEST_PROJECT)
add_subdirectory(UnitTesting)
endif()
