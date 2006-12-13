INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( testcovpredict testcovpredict.cpp )
TARGET_LINK_LIBRARIES( testcovpredict OrcaNavUtil )
ADD_TEST( OrcaNavUtilCovPredict testcovpredict )
MESSAGE( STATUS "Planning to Build Test      : OrcaNavUtilCovPredict [libOrcaNavUtil]")
