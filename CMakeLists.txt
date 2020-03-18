cmake_minimum_required(VERSION 3.5)

project(CPPUtils LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(external/abseil-cpp)
include_directories(external/abseil-cpp)
include_directories(include)

OPTION(DISABLE_TESTING "Disable testing" OFF)
if (NOT DISABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
