set( build TRUE )
set( exe_name CliSegwayRmp )

set( dep_libs OrcaRmpUtil HydroSegwayRmpAcfrCan HydroSegwayRmpFake HydroHumanInput2dKbdTermio )
GBX_REQUIRE_LIBS( build EXE ${exe_name} ${dep_libs} )

if ( build )

  add_executable( clisegwayrmp clisegwayrmp.cpp segwayrmp )
  target_link_libraries( clisegwayrmp ${dep_libs} )

endif ( build )