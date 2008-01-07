SET ( COMP_NAME orca2yarp )
SET ( COMP_NAMESPACE ${COMP_NAME} )

REQUIRE_OPTION( EXE ${COMP_NAME} OFF )
# only tested on Linux but should work where Yarp works.
REQUIRE_VAR( EXE ${COMP_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # not checking for existence and location
    # for this reason this example is OFF by default
    SET( YARP_ROOT /opt/yarp )
    SET( YARP_INCLUDE_DIRS ${YARP_ROOT}/include )
    SET( YARP_LINK_DIRS ${YARP_ROOT}/lib )
    SET( YARP_LIBS YARP_OS ACE )
    
    INCLUDE_DIRECTORIES( ${YARP_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${YARP_LINK_DIRS} )

    # Yarp send/receive examples
    GLOBAL_ADD_EXECUTABLE( yarp_simple_sender simple_sender )
    TARGET_LINK_LIBRARIES( yarp_simple_sender ${YARP_LIBS} )
    
    GLOBAL_ADD_EXECUTABLE( yarp_simple_receiver simple_receiver )
    TARGET_LINK_LIBRARIES( yarp_simple_receiver ${YARP_LIBS} )

    # Orca-to-Yarp example 
    WRAP_COMPONENT_INTO_APP( ${COMP_NAME} ${COMP_NAMESPACE} component mainthread )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${YARP_LIBS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
