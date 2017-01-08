# CMake build control file for safe numerics Library Examples

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
  set_target_properties(${base_name} PROPERTIES FOLDER "example")
  target_compile_options(${base_name} PUBLIC "-std=c++14")
endforeach(file_name)

function( test_example base_name )
  add_test(NAME ${base_name} COMMAND ${base_name})
endfunction(test_example)

test_example(example1)
test_example(example2)
test_example(example3)
test_example(example4)
test_example(example5)
test_example(example6)
test_example(example7)
# test_example(example81)
test_example(example82)
test_example(example83)
#test_example(example84)
test_example(example91)
test_example(example92)
# test_example(example93)
test_example(example10)

set_target_properties(example81 example84 example93 PROPERTIES
    WILL_FAIL TRUE
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
