# CMake build control file for safe numerics Library Examples

cmake_minimum_required(VERSION 3.0)
project("SafeIntegers")

include("CMake/CMakeLists.txt")

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("include")

add_subdirectory("include")
add_subdirectory("examples")
add_subdirectory("test")

