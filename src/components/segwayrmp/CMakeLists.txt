OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( segwayrmp "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

IF ( BUILD AND NOT BUILD_SEGWAYRMP )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( segwayrmp "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_SEGWAYRMP )

IF ( BUILD AND NOT HYDRODLL_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( segwayrmp "libHydroDll not installed" )
ENDIF ( BUILD AND NOT HYDRODLL_INSTALLED )


IF ( BUILD )

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

ENDIF ( BUILD )
