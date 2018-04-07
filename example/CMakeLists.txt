# CMake build control file for safe numerics Library Examples

###########################
# examples

message( STATUS "Runtimes are stored in ${CMAKE_CURRENT_BINARY_DIR}" )

function( test_example base_name )
  message(STATUS ${base_name})
  add_executable(${base_name} ${base_name}.cpp)
  set_target_properties(${base_name} PROPERTIES FOLDER "examples")
  add_test(NAME ${base_name} COMMAND ${base_name})
endfunction(test_example)

test_example(example1)
test_example(example11)
test_example(example2)
test_example(example3)
test_example(example4)
test_example(example5)
test_example(example6)
test_example(example7)
# test_example(example81) # requires console input
test_example(example82)
test_example(example83)
# test_example(example84) # requires console input
test_example(example91)
test_example(example92)
test_example(example10)
test_example(example13)
test_example(example14)
test_example(example15)
test_example(example20)

function( test_compile_fail base_name )
  message(STATUS ${base_name})
  # Add failing-to-compile targets
  add_executable(${base_name} ${base_name}.cpp)

  # Avoid building these targets normally
  set_target_properties(${base_name} PROPERTIES
    FOLDER "examples"
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
  )

  # Add the tests.  These invoke "cmake --build ..." which is a
  # cross-platform way of building the given target.
  add_test(NAME ${base_name}
    COMMAND ${CMAKE_COMMAND} --build . --target ${base_name} --config $<CONFIGURATION>
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  # Expect these tests to fail
  # (i.e. cmake --build should return a non-zero value)
  set_tests_properties(${base_name} PROPERTIES WILL_FAIL TRUE)
endfunction( test_compile_fail base_name )

test_compile_fail(example93)

# end examples targets
####################

###########################
# add misc files to IDE

file(GLOB misc_files 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
add_custom_target(miscellaneous SOURCES ${misc_files})
set_target_properties(miscellaneous PROPERTIES FOLDER "examples")

# end headers in IDE
####################
