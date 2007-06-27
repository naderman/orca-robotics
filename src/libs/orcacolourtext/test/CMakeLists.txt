INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
ADD_EXECUTABLE( colourtest colourtest.cpp )
TARGET_LINK_LIBRARIES( colourtest OrcaColourText )

