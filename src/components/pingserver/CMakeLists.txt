OPTION( BUILD_PINGSERVER "Try to build PingServer component" ON )

IF ( BUILD_PINGSERVER )

    # Look for liboping -- library for 'ping'ing from C
    INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )

    IF ( NOT LIBOPING_FOUND )
        MESSAGE( STATUS "Not Planning to Build Component: pingserver -- liboping not found")
    ELSE ( NOT LIBOPING_FOUND )
    
        INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
        FILE ( GLOB SRCS *.cpp )
    
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pingserver ${SRCS} )
        TARGET_LINK_LIBRARIES( pingserver HydroPing )
    
        GENERATE_CONFIG_FILE( pingserver.def )
    
    ENDIF ( NOT LIBOPING_FOUND )

ENDIF ( BUILD_PINGSERVER )
