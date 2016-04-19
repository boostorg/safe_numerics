# CMake build control file for safe numerics Library Examples

cmake_minimum_required(VERSION 3.0)
project("SafeIntegers")

include("CMake/CMakeLists.txt")

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("include")

# create target to run test andpwd examples
include (CTest)

add_subdirectory("include")
add_subdirectory("examples")
add_subdirectory("test")

