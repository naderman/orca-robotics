SET ( COMP_NAME propertyserver )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
