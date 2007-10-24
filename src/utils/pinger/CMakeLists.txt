OPTION( BUILD_PINGER "Try to build pinger" OFF )

IF ( BUILD_PINGER )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
   
    FIND_COMPONENT_SOURCES( COMP_SRCS )
#     SET( COMP_DEPS )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pinger ${COMP_SRCS} main )
#     TARGET_LINK_LIBRARIES( pinger ${COMP_DEPS} )
    
    # haven't figured out how to build shared libs under win yet
    IF ( NOT OS_WIN )    
        IF ( BUILD_SERVICES )
        # build IceBox service
        GLOBAL_ADD_LIBRARY( OrcaPinger SHARED ${COMP_SRCS} service )
#         TARGET_LINK_LIBRARIES( OrcaPinger ${COMP_DEPS} ) 
        ENDIF ( BUILD_SERVICES )
    ENDIF ( NOT OS_WIN )

  GENERATE_CONFIG_FILE( pinger.def )

ENDIF ( BUILD_PINGER )
