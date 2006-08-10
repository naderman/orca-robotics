OPTION( BUILD_OGNODE "Try to build OgNode" ON )

IF ( BUILD_OGNODE )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )           
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/utils )
    
    GLOBAL_ADD_EXECUTABLE( ognode ${SRCS} )
    TARGET_LINK_LIBRARIES( ognode OrcaOgFusion OrcaOgMap )
    
    GENERATE_CONFIG_FILE( ognode.def )

ENDIF ( BUILD_OGNODE )
