OPTION( BUILD_PURE_ICE "Try to build PureIce Example" ON )

IF ( BUILD_PURE_ICE )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( pureice ${SRCS} )

ENDIF ( BUILD_PURE_ICE )
