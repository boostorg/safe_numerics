# CMake build control file for safe numerics Library Examples

cmake_minimum_required(VERSION 3.6)

project("SafeIntegers")
set (CMAKE_CXX_STANDARD 14)

enable_language(CXX)
include("CMake/CMakeLists.txt")

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("include")

enable_testing()

add_subdirectory("include")
add_subdirectory("examples")
add_subdirectory("test")

