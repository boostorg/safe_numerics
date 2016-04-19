# CMake build control file for safe numerics Library tests

cmake_minimum_required(VERSION 3.0)
project("SafeIntegersTest")

include("../CMake/CMakeLists.txt" )

include_directories("${Boost_INCLUDE_DIRS}")
include_directories("../include")

###########################
# testing and submitting test results to the test dashboard

include (CTest)

if(0)

## Create a file named CTestConfig.cmake adjacent to the current file.
## This new file should contain the following:

set(CTEST_PROJECT_NAME "Safe Numerics")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
# set(CTEST_DROP_LOCATION "/cdash/submit.php?project=MyProject")
set(CTEST_DROP_LOCATION "/index.php?project=Safe+Numerics")
set(CTEST_DROP_SITE_CDASH TRUE)

endif()

###########################
# test targets

file(GLOB test_list
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
foreach(file_name ${test_list})
  string(REPLACE ".cpp" "" base_name ${file_name})
  message(STATUS ${base_name})
  add_executable(${base_name} ${file_name})
  add_test(NAME ${base_name} COMMAND ${base_name})
  set_target_properties(${base_name} PROPERTIES FOLDER "tests")
endforeach(file_name)

# end test targets
####################
