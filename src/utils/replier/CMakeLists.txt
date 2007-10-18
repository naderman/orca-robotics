OPTION( BUILD_REPLIER "Try to build replier" OFF )

IF ( BUILD_REPLIER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
#     SET( COMP_DEPS )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( replier ${COMP_SRCS} main )
#     TARGET_LINK_LIBRARIES( replier ${COMP_DEPS} )
    
    # haven't figured out how to build shared libs under win yet
    IF ( NOT OS_WIN )    
        # build IceBox service
        IF ( BUILD_SERVICES )
            GLOBAL_ADD_LIBRARY( OrcaReplier SHARED ${COMP_SRCS} service )
#             TARGET_LINK_LIBRARIES( OrcaReplier ${COMP_DEPS} ) 
        ENDIF ( BUILD_SERVICES )
    ENDIF ( NOT OS_WIN )

    GENERATE_CONFIG_FILE( replier.def )

ENDIF ( BUILD_REPLIER )
