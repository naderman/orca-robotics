OPTION( BUILD_LASER_MONITOR "Try to build Laser Monitor" ON )

IF ( BUILD_LASER_MONITOR )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( lasermon ${SRCS} )
    
    GENERATE_CONFIG_FILE( lasermon.def )

ENDIF ( BUILD_LASER_MONITOR )
