OPTION( BUILD_LASER2OG "Try to build Laser2Og" ON )

SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( laser2og "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

IF ( BUILD AND NOT BUILD_LASER2OG )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( laser2og "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_LASER2OG )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS OrcaOgFusion OrcaPathPlan )
    
    GLOBAL_ADD_EXECUTABLE( laser2og ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( laser2og ${COMP_DEPS}  )

    GENERATE_CONFIG_FILE( laser2og.def )

ENDIF ( BUILD )
