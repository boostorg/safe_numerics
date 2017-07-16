# CMake build control file for safe numerics Library tests

###########################
# test targets

message( STATUS "Runtimes are stored in ${CMAKE_CURRENT_BINARY_DIR}" )

file(GLOB test_list
  LIST_DIRECTORIES false
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "*.cpp"
)

# CMake doesn't work well for some tests
list(REMOVE_ITEM test_list test_performance.cpp test_trap.cpp)

foreach(file_name ${test_list})
  string(REPLACE ".cpp" "" base_name ${file_name})
  message(STATUS ${base_name})
  add_executable(${base_name} ${file_name})
  add_test(NAME ${base_name} COMMAND ${base_name})
  set_target_properties(${base_name} PROPERTIES FOLDER "tests")
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
