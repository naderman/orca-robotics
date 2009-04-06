include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

# this test has external dependencies
set( build TRUE )
set( exe_name simple )
set( dep_libs HydroMapLoad )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} HYDROMAPLOAD_INSTALLED "libHydroMapLoad not installed" )

if( build )

  add_executable( simple simple.cpp )
  target_link_libraries( simple OrcaPotrace ${dep_libs} )
  GBX_ADD_TEST( OrcaPotraceSimple simple testmap.png )

endif( build )
