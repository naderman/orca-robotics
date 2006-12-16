OPTION( BUILD_ICEPING "Try to build the iceping utility" ON )

IF ( BUILD_ICEPING )

	#
	# This utility is intentionally built without using libOrcaIce.
	# We do not include the standard component compile options.
	#

    ADD_DEFINITIONS( "-Wall" )
    INCLUDE_DIRECTORIES( ${ICE_HOME}/include )
    LINK_DIRECTORIES( ${ICE_HOME}/lib )
    LINK_LIBRARIES( Ice IceUtil )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( iceping ${SRCS} )

ENDIF ( BUILD_ICEPING )
