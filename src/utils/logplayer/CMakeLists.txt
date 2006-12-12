OPTION( BUILD_LOGPLAYER "Try to build LogPlayer" ON )

IF ( NOT WIN32 AND BUILD_LOGPLAYER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( logplayer ${SRCS} )
    TARGET_LINK_LIBRARIES( logplayer OrcaLog OrcaNavUtil OrcaDynamicLoad )

    GENERATE_CONFIG_FILE( logplayer.def )
    
ENDIF ( NOT WIN32 AND BUILD_LOGPLAYER )
