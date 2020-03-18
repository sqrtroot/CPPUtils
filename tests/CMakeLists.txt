cmake_minimum_required(VERSION 3.5)

project(CPPUtils_tests LANGUAGES CXX)

find_package(Catch2 REQUIRED)

add_executable(UtilsTests test.cpp Utils/Tuple.cpp Utils/Stringview.cpp)
set_target_properties(UtilsTests PROPERTIES CMAKE_CXX_STANDARD 17)
target_compile_options(UtilsTests PUBLIC)
target_link_libraries(UtilsTests Catch2::Catch2 absl::strings)

include(Catch)
catch_discover_tests(UtilsTests)
