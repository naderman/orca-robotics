OPTION( BUILD_PINGSERVER "Try to build PingServer component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pingserver "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

IF ( BUILD AND NOT BUILD_PINGSERVER )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pingserver "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_PINGSERVER )

# Look for liboping -- library for 'ping'ing from C
INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )
IF ( BUILD AND NOT LIBOPING_FOUND )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pingserver "liboping not found" )
ENDIF ( BUILD AND NOT LIBOPING_FOUND )

IF ( BUILD AND NOT HYDROPING_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pingserver "libHydroPing not installed" )
ENDIF ( BUILD AND NOT HYDROPING_INSTALLED )


IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( pingserver ${SRCS} )
    TARGET_LINK_LIBRARIES( pingserver HydroPing )

    GENERATE_CONFIG_FILE( pingserver.def )

ENDIF ( BUILD )
