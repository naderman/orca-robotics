GBX_ADD_LICENSE( LGPL )

ORCA_SET_LIBRARY_NAME( HydroMhiPa10Fake )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs ${dep_libs} HydroInterfaces HydroIceUtil ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

include(${ORCA_CMAKE_DIR}/FindNewmat.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} NEWMAT_FOUND "Newmat not found" )

include(${ORCA_CMAKE_DIR}/FindRoboop.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} ROBOOP_FOUND "Roboop not found" )

if( build )

	include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
	include( ${GEARBOX_USE_FILE} )
	include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
	INCLUDE (${ORCA_CMAKE_DIR}/UseNewmat.cmake)
	INCLUDE (${ORCA_CMAKE_DIR}/UseRoboop.cmake)
	
	file( GLOB hdrs *.h )
	file( GLOB srcs *.cpp )
	
	GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
	target_link_libraries( ${LIB_NAME} ${dep_libs} )
	
	GBX_ADD_HEADERS( mhipa10fake ${hdrs} )

endif( build )
