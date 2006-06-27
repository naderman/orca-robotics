OPTION( BUILD_ICEPING "Try to build the iceping utility" ON )

IF ( NOT WIN32 AND BUILD_ICEPING )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
#    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( iceping ${SRCS} )

ENDIF ( NOT WIN32 AND BUILD_ICEPING )
