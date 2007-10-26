OPTION( BUILD_WALKER_CONTROLLER "Try to build ReactiveWalker robot controller" OFF )

IF ( BUILD_WALKER_CONTROLLER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
#     SET( COMP_DEPS )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( reactivewalker ${COMP_SRCS} main )
#     TARGET_LINK_LIBRARIES( reactivewalker ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaReactiveWalker SHARED ${COMP_SRCS} service )
#         TARGET_LINK_LIBRARIES( OrcaReactiveWalker ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( reactivewalker.def )

ENDIF ( BUILD_WALKER_CONTROLLER )
