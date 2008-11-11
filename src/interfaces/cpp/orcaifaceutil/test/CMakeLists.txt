INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceUtil.cmake )

ADD_EXECUTABLE( ifacestringtest ifacestringtest.cpp )
# note that the input file is in the source directory
GBX_ADD_TEST( OrcaIfaceUtil_Test ifacestringtest )
