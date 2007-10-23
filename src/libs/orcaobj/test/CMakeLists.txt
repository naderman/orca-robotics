INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

LINK_LIBRARIES( OrcaObjects OrcaIce )

ADD_EXECUTABLE( stringtest stringtest.cpp )
GLOBAL_ADD_TEST( ObjectStringTest stringtest )

ADD_EXECUTABLE( timetest timetest.cpp )
GLOBAL_ADD_TEST( TimeTest timetest )
