SET ( COMP_NAME registrylist )
SET ( COMP_NAMESPACE ${COMP_NAME} )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    
    WRAP_COMPONENT_INTO_APP( ${COMP_NAME} ${COMP_NAMESPACE} ${COMP_SRCS} )
        
    IF ( NOT OS_WIN )
        TARGET_LINK_LIBRARIES( ${COMP_NAME} IceGrid OrcaCompModel )
    ELSE ( NOT OS_WIN )
        TARGET_LINK_LIBRARIES( ${COMP_NAME} 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaCompModel )
    ENDIF ( NOT OS_WIN )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
