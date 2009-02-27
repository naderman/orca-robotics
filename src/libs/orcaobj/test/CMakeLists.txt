INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )

LINK_LIBRARIES( OrcaObjects )

ADD_EXECUTABLE( stringtest stringtest.cpp )
GBX_ADD_TEST( OrcaObjects_StringTest stringtest )
