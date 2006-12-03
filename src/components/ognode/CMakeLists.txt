OPTION( BUILD_OGNODE "Try to build OgNode" ON )

IF ( NOT WIN32 )
IF ( BUILD_OGNODE )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )           
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )
    
    GLOBAL_ADD_EXECUTABLE( ognode ${SRCS} )
    TARGET_LINK_LIBRARIES( ognode OrcaOgFusion OrcaOgMap )
    
    GENERATE_CONFIG_FILE( ognode.def )

ENDIF ( BUILD_OGNODE )
ENDIF ( NOT WIN32 )
