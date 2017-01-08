# CMake build control file for safe numerics Library tests

#
# Compiler settings - special settings for known compilers
#

message(STATUS "compiler is ${CMAKE_CXX_COMPILER_ID}" )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  add_definitions( -ftemplate-depth=300 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
  add_definitions( /wd4996 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" )
  add_definitions( -ftemplate-depth=300 )
endif()

#
# Project settings
#

find_package(Boost )

if(Boost_FOUND)
  if( CMAKE_HOST_APPLE )
    set(Boost_ADDRESS_MODEL 64 CACHE INTEGER "32/64 bits")
  endif()
  message(STATUS "Boost directories found at ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost version found is ${Boost_VERSION}")
elseif()
    message("Boost NOT Found!")
endif()

message(STATUS "include directories are:" )
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "   ${dir}")
endforeach()

#use folders in organization of the IDE
set(USE_FOLDERS TRUE)

###########################
# testing and submitting test results to the test dashboard

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



