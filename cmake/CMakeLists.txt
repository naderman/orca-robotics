#
# Install all the .cmake files, so other projects can use them.
#
FILE( GLOB SOURCES *.cmake )
INSTALL( PROGRAMS ${SOURCES} DESTINATION cmake )
MESSAGE ( STATUS "Will install CMake scripts into ${PROJECT_INSTALL_DIR}/cmake" )
