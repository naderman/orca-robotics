include( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceLog.cmake )

add_executable( ifacelogtest ifacelogtest.cpp )
# note that the input file is in the source directory
GBX_ADD_TEST( OrcaIfaceLog_Test ifacelogtest )
