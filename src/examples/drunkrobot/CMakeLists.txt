OPTION( BUILD_DRUNK_ROBOT "Try to build Drunk Robot Example" ON )

IF ( NOT WIN32 AND BUILD_DRUNK_ROBOT )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( drunkrobot ${SRCS} )

    GENERATE_CONFIG_FILE( drunkrobot.def )

ENDIF ( NOT WIN32 AND BUILD_DRUNK_ROBOT )
