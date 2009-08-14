include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_executable( colourtest colourtest.cpp )
target_link_libraries( colourtest HydroColourText )

