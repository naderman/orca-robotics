IF( NOT OS_WIN )

OPTION( BUILD_LOGPLAYER "Try to build LogPlayer" ON )

IF( BUILD_LOGPLAYER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( logplayer ${SRCS} )
    TARGET_LINK_LIBRARIES( logplayer OrcaLog OrcaNavUtil HydroDll )

    GENERATE_CONFIG_FILE( logplayer.def )
    
ENDIF( BUILD_LOGPLAYER )
ENDIF( NOT OS_WIN )
