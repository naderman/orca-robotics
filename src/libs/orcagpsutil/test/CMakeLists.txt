INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( testmga testmga.cpp )
TARGET_LINK_LIBRARIES( testmga OrcaGpsUtil )
ADD_TEST( OrcaGpsUtilTestMga testmga )
MESSAGE( STATUS "Planning to Build Test      : OrcaGpsUtilTestMga [libOrcaGpsUtil]")
