INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

ADD_EXECUTABLE( logtest logtest.cpp )
TARGET_LINK_LIBRARIES( logtest OrcaLog )
# note that the input file is in the source directory
ADD_TEST( LibOrcaLogTest logtest )
MESSAGE( STATUS "Planning to Build Test     : LibOrcaLogTest [libOrcaLog]")
