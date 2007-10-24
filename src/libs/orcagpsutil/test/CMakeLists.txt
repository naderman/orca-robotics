INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

LINK_LIBRARIES( OrcaGpsUtil )

ADD_EXECUTABLE( testmga testmga.cpp )
GLOBAL_ADD_TEST( OrcaGpsUtilTestMga testmga )
