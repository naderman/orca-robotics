IF ( OS_LINUX )

OPTION( BUILD_ICEGRIDMON "Try to build IceGridMon" ON )

IF( BUILD_ICEGRIDMON )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIceGrid )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( icegridmon ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( icegridmon ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaIceGridMon SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaIceGridMon ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( icegridmon.def )

ENDIF( BUILD_ICEGRIDMON )
ENDIF( OS_LINUX  )
