# CMake build control file for safe numerics Library Examples

cmake_minimum_required(VERSION 3.0)
project("SafeIntegersExamples")

include("../CMake/CMakeLists.txt" )

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("../include")

###########################
# examples

message( STATUS "Runtimes are stored in ${CMAKE_CURRENT_BINARY_DIR}" )

file(GLOB example_list
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "*.cpp"
)

foreach(file_name ${example_list})
  string(REPLACE ".cpp" "" base_name ${file_name})
  message(STATUS ${base_name})
  add_executable(${base_name} ${file_name})
  add_test(NAME ${base_name} COMMAND ${base_name})
  set_target_properties(${base_name} PROPERTIES FOLDER "example")
endforeach(file_name)

set_target_properties(example84 PROPERTIES
    WILL_FAIL TRUE
)
set_target_properties(example81 example93 PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
)

# end examples targets
####################

###########################
# add misc files to IDE

file(GLOB misc_files 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
add_custom_target(miscellaneous SOURCES ${misc_files})
set_target_properties(miscellaneous PROPERTIES FOLDER "example")

# end headers in IDE
####################
