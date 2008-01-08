SET ( APP_NAME iceping )

REQUIRE_OPTION( EXE ${APP_NAME} ON )

IF ( BUILD )

	#
	# This utility is intentionally built without using libOrcaIce.
	# We do not include the standard component compile options.
	#
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${SRCS} )

    # special case: we don't have a .def file here, installing a sample .cfg
    INSTALL_FILES( /cfg iceping.cfg )

ENDIF ( BUILD )
