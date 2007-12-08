OPTION( BUILD_OGNODE "Try to build OgNode" ON )
SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ognode "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_OGNODE )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ognode "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_OGNODE )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )           
    
    GLOBAL_ADD_EXECUTABLE( ognode ${SRCS} )
    TARGET_LINK_LIBRARIES( ognode OrcaOgFusion OrcaOgMap OrcaIfaceImpl )
    
    GENERATE_CONFIG_FILE( ognode.def )

ENDIF ( BUILD )
