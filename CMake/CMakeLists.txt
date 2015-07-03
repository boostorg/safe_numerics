# CMake build control file for safe numerics Library tests

cmake_minimum_required(VERSION 3.0)

project("SafeIntegers")

#
# Compiler settings - special settings for known compilers
#

message(STATUS "compiler is ${CMAKE_CXX_COMPILER_ID}" )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  add_definitions( -ftemplate-depth=300 )
  set (CMAKE_CXX_FLAGS "-std=c++14" )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
  add_definitions( /wd4996 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" )
  add_definitions( -ftemplate-depth=300 )
  # include the following if and only if you want to use c++14 features
  set (CMAKE_CXX_FLAGS "-std=c++14" )
  set (CMAKE_CXX_FLAGS_DEBUG "-g -O0" )
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3" )
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -dead_strip")
endif()

#
# Locate Project Prerequisites 
#

# Boost

# note: we're assuming that boost has been built with:
# ./b2 —-layout=versioned toolset=clang-darwin link=static,shared variant=debug,release stage

###########################
# special notes for Xcode.

# these three should result in CMake setting the variables
# Boost_SERIALIZATION_LIBRARY_DEBUG … to the correct values.  

# But my current version of CMake doesn't automatically set the library names
# to point to the the libraries to be used.  The variables are created
# but they are not initialized.  So be prepared to set these variables by hand.
# If you want to use the static libraries - point to the boost libraries ending
# in ".a".  If you want to use the shared boost libraries - point to the libraries
# ending in ".dylib".

# But wait - there's more.
# if both lib.a and lib.dylib both exist in the library directory, Xcode will
# automatically chose the *.dylib one - and there's nothing you can do to fix this.
# So my recommendation is 
# a) to place the compiled libraries in two different directories
#    - e.g. stage/lib-static/*.a and stage/lib-shared/*.dylib
#    and set the CMake variable Boost_LIBRARY_DIR to point to one or the other.

# b) create two different CMake build directories - build-static and build-shared
#    and switch between the generated projects as desired.  I like to test both since
#    there are things like dead code elimination and visibility which vary
#    between the two environments.
#
#    caveat - as I write this, I've been unable to get the tests on the shared
#    library to function. Problem is that one needs to either put the shared
#    libraries in a special known place or set an environmental
#    variable which points to the shared library directory.  I prefer the latter
#    but I've been unable to figure out how to get Xcode to do on a global basis
#    and it's not practical to do this for 247 test targets one at a time.

# c) The targets in the project will by default be built as universal 32/64 binaries
#    in debug mode.  Use the Xcode IDE select one or the other type of binaries

# d) If you are creating executables to run under Xcode for testing examples or other
#    purposes AND you're linking to dynamic linked versions of other boost libraries,
#    you'll have to set the environmental variable DYLD_,

# d) If your test code uses other boost libraries which are not header only, You'll
#    have to set the environmental variable DYLD_LIBRARY_PATH to the directory which
#    contains the *.dylib files for Boost libraries.  Unfortunately, it turns out
#    that it's not at all obvious how to do this and as far as I can tell, it can't
#    be done automatically by CMake. Here's what you have to do:
#    1) Using CMake create the Xcode project in the normal way
#    2) Build all your executables.
#    3) If you've imported shared libraries your tests may not fail with the message:
#       dylib: can't find …. 
#    4) Select the target "RUN_TESTS"
#    5) select edit scheme for the target
#    6) add an environmental variable to be set when executing
#       DYLD_LIBRARY_PATH = … directory which contains the boost library shared
#       *.dylib files.
#    7) Try to run tests again.

# end special note for Xcode
############################

#
# Project settings
#

find_package(Boost )

if(Boost_FOUND)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_LIBS ON CACHE BOOL "Link to Boost static libraries")
  # note: it seems that boost builds builds both address models in any case
  # so we can defer this decision to the IDE just as we do for debug/release
  # so we'll not use this now
  # set(Boost_ADDRESS_MODEL 64 CACHE INTEGER "32/64 bits")
  if( CMAKE_HOST_APPLE )
    set(Boost_ADDRESS_MODEL 64 CACHE INTEGER "32/64 bits")
  endif()
  message(STATUS "Boost is ${BOOST_ROOT}")
  message(STATUS "Boost directories found at ${Boost_INCLUDE_DIRS}")
  message(STATUS "Boost libraries found at ${Boost_LIBRARY_DIRS}")
  message(STATUS "Boost component libraries to be linked are ${Boost_LIBRARIES}")
  message(STATUS "Boost version found is ${Boost_VERSION}")
  if(Boost_USE_STATIC_LIBS)
    set(BUILD_SHARED_LIBRARIES OFF)
  else()
    set(BUILD_SHARED_LIBRARIES ON)
  endif()
  message(STATUS "Boost Libraries used are: ${Boost_LIBRARIES}" )
elseif()
    message("Boost NOT Found!")
endif()

include_directories("../include" "${Boost_INCLUDE_DIRS}")
link_directories("${Boost_LIBRARY_DIRS}")

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
# library builds

# header library only - nothing to be built
#
# add_library(library_name ../src/sources1.cpp  …)

# end library build
###########################

###########################
# test targets

# the "include (CTest)" above includes enable_testing() 
# so the following line isn't necessary
# enable_testing()

file(GLOB test_list
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../test/*.cpp"
)
foreach(file_path ${test_list})
  string(REPLACE "../test/" "" file_name ${file_path})
  string(REPLACE ".cpp" "" base_name ${file_name})
  message(STATUS ${base_name})
  add_executable(${base_name} ${file_path})
  # target_link_libraries(${base_name}  ${Boost_LIBRARIES} )
  add_test(NAME ${base_name} COMMAND ${base_name})
endforeach(file_path)

# end test targets
####################

###########################
# examples

file(GLOB example_list
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../examples/*.cpp"
)
foreach(file_path ${example_list})
  string(REPLACE "../examples/" "" file_name ${file_path})
  string(REPLACE ".cpp" "" base_name ${file_name})
  message(STATUS ${base_name})
  add_executable(${base_name} ${file_path})
  # target_link_libraries(${base_name}  ${Boost_LIBRARIES} )
  add_test(NAME ${base_name} COMMAND ${base_name})
endforeach(file_path)

# end examples targets
####################

####################
# add include headers to IDE

file(GLOB include_files 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.hpp"
)
add_custom_target(include SOURCES ${include_files})

file(GLOB include_concept_files
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/concept/*.hpp"
)
add_custom_target(include_concept_files SOURCES ${include_concept_files})

# end headers in IDE
####################

