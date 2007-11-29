IF ( OS_LINUX )

OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

IF ( BUILD_SEGWAYRMP )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroDll HydroInterfaces OrcaIfaceImpl OrcaObjectUtil OrcaRobotDriverUtil )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( segwayrmp ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( segwayrmp ${COMP_DEPS} )

    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaSegwayRmp SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaSegwayRmp ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( segwayrmp.def )

ENDIF ( BUILD_SEGWAYRMP )
ENDIF ( OS_LINUX )
