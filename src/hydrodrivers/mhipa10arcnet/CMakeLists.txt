GBX_ADD_LICENSE( LGPL )

ORCA_SET_LIBRARY_NAME( HydroMhiPa10Arcnet )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs GbxUtilAcfr GbxIceUtilAcfr HydroInterfaces HydroIceUtil ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

include(${ORCA_CMAKE_DIR}/FindPcap.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} PCAP_FOUND "pcap.h not found" )
set( dep_libs ${dep_libs} pcap )

if( build )

	include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
	include( ${GEARBOX_USE_FILE} )
	include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
	
	file( GLOB hdrs *.h )
	file( GLOB srcs *.cpp )
	
	GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
	target_link_libraries( ${LIB_NAME} ${dep_libs} )
	if( ORCA_BUILD_TESTS )
	    add_subdirectory( test )
	endif( ORCA_BUILD_TESTS )
	
	GBX_ADD_HEADERS( mhipa10arcnet ${hdrs} )

endif( build )
