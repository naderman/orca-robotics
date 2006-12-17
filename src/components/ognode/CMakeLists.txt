IF ( NOT OS_WIN )

OPTION( BUILD_OGNODE "Try to build OgNode" ON )

IF ( BUILD_OGNODE )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )           
    
    GLOBAL_ADD_EXECUTABLE( ognode ${SRCS} )
    TARGET_LINK_LIBRARIES( ognode OrcaOgFusion OrcaOgMap )
    
    GENERATE_CONFIG_FILE( ognode.def )

ENDIF ( BUILD_OGNODE )
ENDIF ( NOT OS_WIN )
