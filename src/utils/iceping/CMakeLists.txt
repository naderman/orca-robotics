OPTION( BUILD_ICEPING "Try to build the iceping utility" ON )

IF ( BUILD_ICEPING )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( iceping ${SRCS} )

ENDIF ( BUILD_ICEPING )
