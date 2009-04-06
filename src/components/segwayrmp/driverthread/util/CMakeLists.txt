ORCA_SET_COMPONENT_NAME( SegwayRmpDriverThread )

set( dep_libs HydroSegwayRmpAcfrCan SegwayRmpDriverThread )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

  include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

  GBX_ADD_EXECUTABLE( segwaydriverthread segwaydriverthread.cpp )
  target_link_libraries( segwaydriverthread ${dep_libs} )

endif( build )