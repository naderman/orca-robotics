SET( APP_NAME iceping )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

IF( build )

	#
	# This utility is intentionally built without using libOrcaIce.
	# We do not include the standard component compile options.
	#
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )

    # special case: we don't have a .def file here, installing a sample .cfg
    GBX_ADD_SHARED_FILES( cfg iceping.cfg )

ENDIF( build )
