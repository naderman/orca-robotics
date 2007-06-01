OPTION( BUILD_DRUNK_ROBOT "Try to build Drunk Robot Example" OFF )

IF ( NOT OS_WIN AND BUILD_DRUNK_ROBOT )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( drunkrobot ${SRCS} )

    GENERATE_CONFIG_FILE( drunkrobot.def )

ENDIF ( NOT OS_WIN AND BUILD_DRUNK_ROBOT )
