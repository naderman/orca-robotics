OPTION( BUILD_ORCA_MONITOR "Try to build Orca Monitor" ON )
SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( orcamon "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_ORCA_MONITOR )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( orcamon "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_ORCA_MONITOR )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( orcamon ${SRCS} )
    
    GENERATE_CONFIG_FILE( orcamon.def )

ENDIF ( BUILD )
