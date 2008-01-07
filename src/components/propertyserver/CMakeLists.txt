SET ( COMP_NAME propertyserver )
SET ( COMP_NAMESPACE propserver )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${COMP_NAME} ${COMP_NAMESPACE} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
