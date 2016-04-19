####################
# add include headers to IDE

file(GLOB include_files 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
add_custom_target(safe_numerics SOURCES ${include_files})
set_target_properties(safe_numerics PROPERTIES FOLDER "includes")

add_subdirectory("concept")

# end headers in IDE
####################
