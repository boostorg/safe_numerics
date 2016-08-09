# CMake build control file for safe numerics Library tests

cmake_minimum_required(VERSION 3.0)
project("SafeIntegersTest")

include("../CMake/CMakeLists.txt" )

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("../include")

###########################
# test targets

message( STATUS "Runtimes are stored in ${CMAKE_CURRENT_BINARY_DIR}" )

file(GLOB test_list
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "*.cpp"
)
foreach(file_name ${test_list})
  string(REPLACE ".cpp" "" base_name ${file_name})
  message(STATUS ${base_name})
  add_executable(${base_name} ${file_name})
  add_test(NAME ${base_name} COMMAND ${base_name})
  set_target_properties(${base_name} PROPERTIES FOLDER "tests")
  target_compile_options(${base_name} PUBLIC "-std=c++14")
endforeach(file_name)

# end test targets
####################

###########################
# add headers to IDE

file(GLOB include_files 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
add_custom_target(include SOURCES ${include_files})
set_target_properties(include PROPERTIES FOLDER "tests")

# end headers in IDE
####################
