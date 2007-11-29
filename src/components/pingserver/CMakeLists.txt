IF ( OS_LINUX )

OPTION( BUILD_PINGSERVER "Try to build PingServer component" ON )

IF ( BUILD_PINGSERVER )

    # Look for liboping -- library for 'ping'ing from C
    INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )

    IF ( LIBOPING_FOUND )
    
        INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
        FILE ( GLOB SRCS *.cpp )
    
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pingserver ${SRCS} )
        TARGET_LINK_LIBRARIES( pingserver HydroPing )
    
        GENERATE_CONFIG_FILE( pingserver.def )
    
    ELSE ( LIBOPING_FOUND )
        GLOBAL_NOT_ADD_EXECUTABLE( pingserver "liboping not found" )
    ENDIF ( LIBOPING_FOUND )

ENDIF ( BUILD_PINGSERVER )
ENDIF ( OS_LINUX )
