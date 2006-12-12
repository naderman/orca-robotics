OPTION( BUILD_LOGGER "Try to build Logger" ON )

IF( NOT WIN32 AND BUILD_LOGGER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
      component
    )

    # stand-alone component
    GLOBAL_ADD_EXECUTABLE( logger main ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( logger OrcaLog OrcaDynamicLoad )
    
    # IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLoggerService SHARED service ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( OrcaLoggerService OrcaLog )

    GENERATE_CONFIG_FILE( logger.def )

ENDIF( NOT WIN32 AND BUILD_LOGGER )
