INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

ADD_EXECUTABLE( propfiletest propfiletest.cpp )
GBX_ADD_TEST( OrcaIcePropFileTest propfiletest )

