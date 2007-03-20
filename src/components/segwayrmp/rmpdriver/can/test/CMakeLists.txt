#Use the default include stuff 
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# Glob all source files
FILE( GLOB SRCS *.cpp )

#Now tell the system where to find the libraries we need to link
LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/src/components/segwayrmp/peakcan/)

#Tell the system to build a standalone app for us to use for testing
ADD_EXECUTABLE( TestCanBin main.cpp )

#The target needs to be linked with these libraries
TARGET_LINK_LIBRARIES( TestCanBin OrcaSegwayRmpCan.so)
