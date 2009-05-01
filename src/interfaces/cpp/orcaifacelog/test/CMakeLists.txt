include( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceLog.cmake )

add_executable( ifacelogtest ifacelogtest.cpp )
# note that the input file is in the source directory
GBX_ADD_TEST( OrcaIfaceLog_Test ifacelogtest )

# get_target_property( _debug ifacelogtest LINK_FLAGS )
# get_target_property( _debug ifacelogtest LIBRARY_OUTPUT_DIRECTORY )
# get_target_property( _debug ifacelogtest LOCATION )
# message( STATUS "******* ifacelogtest _debug='${_debug}'" )

# debug
export( TARGETS ifacelogtest FILE ${PROJECT_BINARY_DIR}/debug.cmake )
