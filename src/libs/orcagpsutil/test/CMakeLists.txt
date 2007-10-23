INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

LINK_LIBRARIES( OrcaGpsUtil )

ADD_EXECUTABLE( testmga testmga.cpp )
GLOBAL_ADD_TEST( OrcaGpsUtilTestMga testmga )
