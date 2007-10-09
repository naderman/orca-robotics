OPTION( BUILD_BRICK "Try to build Brick test component" ON )

IF ( BUILD_BRICK )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )

    # libs on which our code depends go here
#     SET( COMP_DEPS ??? )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( brick ${COMP_SRCS} main )
#     TARGET_LINK_LIBRARIES( brick ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaBrick SHARED ${COMP_SRCS} service )
#         TARGET_LINK_LIBRARIES( OrcaBrick ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( brick.def )

ENDIF ( BUILD_BRICK )

