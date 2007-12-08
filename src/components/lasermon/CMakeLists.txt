OPTION( BUILD_LASER_MONITOR "Try to build Laser Monitor" ON )
SET ( BUILD TRUE )

IF ( BUILD AND NOT BUILD_LASER_MONITOR )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( lasermon "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_LASER_MONITOR )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( lasermon ${COMP_SRCS} )
    
    GENERATE_CONFIG_FILE( lasermon.def )

ENDIF ( BUILD )
