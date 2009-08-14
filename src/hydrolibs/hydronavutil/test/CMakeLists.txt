# include( ${ORCA_CMAKE_DIR}/component_rules.cmake )
# include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

link_libraries( HydroNavUtil )

add_executable( testoffset testoffset.cpp )
GBX_ADD_TEST( HydroNavUtilOffset testoffset )

add_executable( testcovpredict testcovpredict.cpp )
GBX_ADD_TEST( HydroNavUtilCovPredict testcovpredict )

add_executable( testcov3d testcov3d.cpp )
GBX_ADD_TEST( HydroNavUtilCov3d testcov3d )

add_executable( testchisq testchisq.cpp )
GBX_ADD_TEST( HydroNavUtilChiSquare testchisq )
