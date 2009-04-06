set( APP_NAME iceping )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

if( build )

	#
	# This utility is intentionally built without using libOrcaIce.
	# We do not include the standard component compile options.
	#
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    file( GLOB srcs *.cpp )
    
    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )

    # special case: we don't have a .def file here, installing a sample .cfg
    GBX_ADD_SHARED_FILES( cfg iceping.cfg )

endif( build )
