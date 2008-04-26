INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceString.cmake )

ADD_EXECUTABLE( ifacestringtest ifacestringtest.cpp )
# note that the input file is in the source directory
GBX_ADD_TEST( OrcaIfaceStringTest ifacestringtest )
