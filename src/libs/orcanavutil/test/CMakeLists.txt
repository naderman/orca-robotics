INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( testcovpredict testcovpredict.cpp )
TARGET_LINK_LIBRARIES( testcovpredict OrcaNavUtil )
ADD_TEST( OrcaNavUtilCovPredict testcovpredict )
MESSAGE( STATUS "Planning to Build Test      : OrcaNavUtilCovPredict [libOrcaNavUtil]")

ADD_EXECUTABLE( testcov3d testcov3d.cpp )
TARGET_LINK_LIBRARIES( testcov3d OrcaNavUtil )
ADD_TEST( OrcaNavUtilCov3d testcov3d )
MESSAGE( STATUS "Planning to Build Test      : OrcaNavUtilCov3d [libOrcaNavUtil]")

ADD_EXECUTABLE( testchisq testchisq.cpp )
TARGET_LINK_LIBRARIES( testchisq OrcaNavUtil )
ADD_TEST( OrcaNavUtilChiSquare testchisq )
MESSAGE( STATUS "Planning to Build Test      : OrcaNavUtilChiSquare [libOrcaNavUtil]")
