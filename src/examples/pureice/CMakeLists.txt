OPTION( BUILD_PURE_ICE "Try to build PureIce Example" ON )

IF ( NOT WIN32 AND BUILD_PURE_ICE )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( pureice ${SRCS} )

ENDIF ( NOT WIN32 AND BUILD_PURE_ICE )
