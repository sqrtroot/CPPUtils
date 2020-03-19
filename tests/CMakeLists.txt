cmake_minimum_required(VERSION 3.5)

project(CPPUtils_tests LANGUAGES CXX)


add_executable(UtilsTests test.cpp Utils/Tuple.cpp Utils/Stringview.cpp)
set_target_properties(UtilsTests PROPERTIES CMAKE_CXX_STANDARD 17)
target_include_directories(UtilsTests PUBLIC external/catch2/single_include)
target_compile_options(UtilsTests PUBLIC)
target_link_libraries(UtilsTests absl::strings)
