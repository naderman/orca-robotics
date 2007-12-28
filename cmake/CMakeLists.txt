#
# Install all the .cmake scripts and source templates, so other projects can use them.
#
FILE( GLOB SOURCES *.cmake *.template )
INSTALL( PROGRAMS ${SOURCES} DESTINATION cmake )
MESSAGE ( STATUS "Will install CMake scripts into ${PROJECT_INSTALL_DIR}/cmake" )
