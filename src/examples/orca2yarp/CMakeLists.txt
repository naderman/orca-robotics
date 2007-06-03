# only tested on Linux but should work where Yarp works.
IF ( OS_LINUX )

OPTION( BUILD_ORCA2YARP "Try to build Orca-to-Yarp example" OFF )

IF ( BUILD_ORCA2YARP )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

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
    GLOBAL_ADD_EXECUTABLE( orca2yarp main handler )
    TARGET_LINK_LIBRARIES( orca2yarp ${YARP_LIBS} )

    GENERATE_CONFIG_FILE( orca2yarp.def )

ENDIF ( BUILD_ORCA2YARP )

ENDIF ( OS_LINUX )
