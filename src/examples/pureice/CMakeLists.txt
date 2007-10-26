OPTION( BUILD_PURE_ICE "Try to build PureIce Example" ON )

IF ( NOT OS_WIN AND BUILD_PURE_ICE )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( pureice ${SRCS} )

ENDIF ( NOT OS_WIN AND BUILD_PURE_ICE )
