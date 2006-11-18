#
# Install all the .cmake files, so other projects can use them.
#
INSTALL_FILES( /cmake .*\\.cmake$ )
MESSAGE ( STATUS "Will install CMake scripts into ${PROJECT_INSTALL_DIR}/cmake" )
