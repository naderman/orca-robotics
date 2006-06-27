OPTION( BUILD_DRUNK_ROBOT "Try to build Drunk Robot Example" ON )

IF ( NOT WIN32 AND BUILD_DRUNK_ROBOT )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( drunkrobot ${SRCS} )

    GENERATE_FROM_DEF( drunkrobot.def )

ENDIF ( NOT WIN32 AND BUILD_DRUNK_ROBOT )
