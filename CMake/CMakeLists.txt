# CMake build control file for Serialization Library tests

cmake_minimum_required(VERSION 2.8)

project("SafeIntegers")

#
# Compiler settings - special settings for known compilers
#

message(STATUS "compiler is ${CMAKE_CXX_COMPILER_ID}" )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  add_definitions( -ftemplate-depth=300 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
  add_definitions( /wd4996 )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  add_definitions( -ftemplate-depth=300 )
  # include the following if and only if you want to use c++11 features
  set (CMAKE_CXX_FLAGS "-std=c++11" )
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
#    and set the CMake variable Boost_LIBRARY_DIR to point to one or the other
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
#    in debug mode.  When working with the  

# end special note for Xcode
############################

#
# Project settings
#

find_package(Boost REQUIRED COMPONENTS )

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
# library builds

# header library only - nothing to be built
#
# add_library(library_name ../src/sources1.cpp  …)

# end library build
###########################

###########################
# testing and submitting test results

include (CTest)

if(0)

## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set(CTEST_PROJECT_NAME "Safe Numerics")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
# set(CTEST_DROP_LOCATION "/cdash/submit.php?project=MyProject")
set(CTEST_DROP_LOCATION "/index.php?project=Safe+Numerics")
set(CTEST_DROP_SITE_CDASH TRUE)

endif()

# the include above includes enable_testing() so the following line isn't
# necessary
# enable_testing()

if(0)

message(STATUS "test_cast")
add_executable( test_cast ../test/test_cast.cpp )
target_link_libraries( test_cast ${Boost_LIBRARIES} )
add_test(NAME test_cast COMMAND test_cast)

message(STATUS "test_add")
add_executable( test_add ../test/test_add.cpp )
target_link_libraries( test_add  ${Boost_LIBRARIES} )
add_test(NAME test_add COMMAND test_add)

message(STATUS "test_subtract")
add_executable( test_subtract ../test/test_subtract.cpp )
target_link_libraries( test_subtract  ${Boost_LIBRARIES} )
add_test(NAME test_subtract COMMAND test_subtract)

message(STATUS "test_multiply")
add_executable( test_multiply ../test/test_multiply.cpp )
target_link_libraries( test_multiply  ${Boost_LIBRARIES} )
add_test(NAME test_multiply COMMAND test_multiply)

message(STATUS "test_divide")
add_executable( test_divide ../test/test_divide.cpp )
target_link_libraries( test_divide  ${Boost_LIBRARIES} )
add_test(NAME test_divide COMMAND test_divide)

message(STATUS "test_modulus")
add_executable( test_modulus ../test/test_modulus.cpp )
target_link_libraries( test_modulus ${Boost_LIBRARIES} )
add_test(NAME test_modulus COMMAND test_modulus)

message(STATUS "test_compare")
add_executable( test_compare ../test/test_compare.cpp)
target_link_libraries( test_compare  ${Boost_LIBRARIES} )
add_test(NAME test_compare COMMAND test_compare)

message(STATUS "test_conversion")
add_executable( test_conversion ../test/test_conversion.cpp)
target_link_libraries( test_conversion  ${Boost_LIBRARIES} )
add_test(NAME test_conversion COMMAND test_conversion)

message(STATUS "test")
add_executable( test ../test/test.cpp)
target_link_libraries( test  ${Boost_LIBRARIES} )
add_test(NAME test COMMAND test)

# examples
message(STATUS "example1")
add_executable( example1 ../examples/example1.cpp)
target_link_libraries( example1  ${Boost_LIBRARIES} )
add_test(NAME example1 COMMAND example1)

message(STATUS "example2")
add_executable( example2 ../examples/example2.cpp)
target_link_libraries( example2 ${Boost_LIBRARIES} )
add_test(NAME example2 COMMAND example2)

message(STATUS "example3")
add_executable( example3 ../examples/example3.cpp)
target_link_libraries( example3  ${Boost_LIBRARIES} )
add_test(NAME example3 COMMAND example3)

message(STATUS "example4")
add_executable( example4 ../examples/example4.cpp)
target_link_libraries( example4  ${Boost_LIBRARIES} )
add_test(NAME example4 COMMAND example4)

endif()

file(GLOB test_files
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../test/*.cpp"
)
foreach(test_file_path ${test_files} )
    set(test_file_name get_filename_component( NAME ${test_file_path} ) )
    message(STATUS "${test_file_path"})
    add_executable( ${test_file_path} ../test/${test_file_path}.cpp)
    target_link_libraries( ${test_file_path}  ${Boost_LIBRARIES} )
    add_test(NAME${test_file_path} COMMAND ${test_file_path})
endforeach(t)

# end test targets
####################

####################
# add include headers to IDE

file(GLOB include_files
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.hpp"
)

file(GLOB include_concept_files
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/concept/*.hpp"
)
add_custom_target(include_concept_files SOURCES ${include_concept_files})

# end headers in IDE
####################

