cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the project name
project(MLEngineProject VERSION 1.0)

add_subdirectory(MLEngine)
add_subdirectory(Test)
add_subdirectory(UnitTesting)
