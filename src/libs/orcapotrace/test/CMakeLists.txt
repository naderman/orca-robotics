INCLUDE( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

# this test has external dependencies
SET( build TRUE )
SET( exe_name simple )
SET( dep_libs HydroMapLoad )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} HYDROMAPLOAD_INSTALLED "libHydroMapLoad not installed" )

IF( build )

  ADD_EXECUTABLE( simple simple.cpp )
  TARGET_LINK_LIBRARIES( simple OrcaPotrace ${dep_libs} )
  GBX_ADD_TEST( OrcaPotraceSimple simple testmap.png )

ENDIF( build )
