ORCA_SET_COMPONENT_NAME( Orca2Yarp )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} OFF )
# only tested on Linux but should work where Yarp works.
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

IF( build)

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
    GBX_ADD_EXECUTABLE( yarp_simple_sender simple_sender )
    TARGET_LINK_LIBRARIES( yarp_simple_sender ${YARP_LIBS} )
    
    GBX_ADD_EXECUTABLE( yarp_simple_receiver simple_receiver )
    TARGET_LINK_LIBRARIES( yarp_simple_receiver ${YARP_LIBS} )

    # Orca-to-Yarp example 
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} mainthread )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${YARP_LIBS} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
