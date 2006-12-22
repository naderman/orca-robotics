IF( NOT OS_WIN )

OPTION( BUILD_LOGGER "Try to build Logger" ON )

IF( BUILD_LOGGER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( LoggerSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( logger main )
    TARGET_LINK_LIBRARIES( logger LoggerSources OrcaLog OrcaDynamicLoad )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLogger SHARED service )
    TARGET_LINK_LIBRARIES( OrcaLogger LoggerSources OrcaLog OrcaDynamicLoad )

    GENERATE_CONFIG_FILE( logger.def )

ENDIF( BUILD_LOGGER )
ENDIF( NOT OS_WIN )
