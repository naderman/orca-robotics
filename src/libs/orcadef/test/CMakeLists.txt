INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( deftest deftest.cpp )
TARGET_LINK_LIBRARIES( deftest OrcaDef )
# note that the input file is in the source directory
ADD_TEST( LibOrcaDefTest deftest ${PROJECT_SOURCE_DIR}/src/libs/orcadef/test/def_test_input.def )
MESSAGE( STATUS "Planning to Build Test     : OrcaDef [libOrcaDef]")
