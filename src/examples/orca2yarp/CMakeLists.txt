ORCA_SET_COMPONENT_NAME( Orca2Yarp )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} OFF )
# only tested on Linux but should work where Yarp works.
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

if( build)

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # not checking for existence and location
    # for this reason this example is OFF by default
    set( YARP_ROOT /opt/yarp )
    set( YARP_INCLUDE_DIRS ${YARP_ROOT}/include )
    set( YARP_LINK_DIRS ${YARP_ROOT}/lib )
    set( YARP_LIBS YARP_OS ACE )
    
    include_directories( ${YARP_INCLUDE_DIRS} )
    link_directories( ${YARP_LINK_DIRS} )

    # Yarp send/receive examples
    GBX_ADD_EXECUTABLE( yarp_simple_sender simple_sender )
    target_link_libraries( yarp_simple_sender ${YARP_LIBS} )
    
    GBX_ADD_EXECUTABLE( yarp_simple_receiver simple_receiver )
    target_link_libraries( yarp_simple_receiver ${YARP_LIBS} )

    # Orca-to-Yarp example 
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} mainthread )
    target_link_libraries( ${APP_NAME} ${YARP_LIBS} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
