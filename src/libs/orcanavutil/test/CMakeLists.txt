INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR}/src/libs
  )

LINK_DIRECTORIES( 
  ${PROJECT_BINARY_DIR}/src/utils/orcanavutil
  )

ADD_EXECUTABLE( testcovpredict testcovpredict.cpp )
TARGET_LINK_LIBRARIES( testcovpredict OrcaNavUtil )
ADD_TEST( OrcaNavUtilCovPredict testcovpredict )
MESSAGE( STATUS "Planning to Build Test     : OrcaNavUtilCovPredict [libOrcaNavUtil]")
