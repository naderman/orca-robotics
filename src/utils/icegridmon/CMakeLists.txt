IF ( OS_LINUX )

OPTION( BUILD_ICEGRIDMON "Try to build IceGridMon" ON )

IF( BUILD_ICEGRIDMON )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( IceGridMonSources )
    
    SET( DEP_LIBS 
      OrcaIceGrid
      OrcaMisc )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( icegridmon main )
    TARGET_LINK_LIBRARIES( icegridmon IceGridMonSources ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaIceGridMon SHARED service )
    TARGET_LINK_LIBRARIES( OrcaIceGridMon IceGridMonSources ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( icegridmon.def )

ENDIF( BUILD_ICEGRIDMON )
ENDIF( OS_LINUX  )
