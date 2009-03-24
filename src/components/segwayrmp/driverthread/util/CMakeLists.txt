ORCA_SET_COMPONENT_NAME( SegwayRmpDriverThread )

SET( int_libs SegwayRmpDriverThread )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

SET( ext_libs HydroSegwayRmpAcfrCan )
GBX_REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

SET( dep_libs ${int_libs} ${ext_libs} )

IF ( build )

  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

  GBX_ADD_EXECUTABLE( segwaydriverthread segwaydriverthread.cpp )
  TARGET_LINK_LIBRARIES( segwaydriverthread ${dep_libs} )

ENDIF( build )