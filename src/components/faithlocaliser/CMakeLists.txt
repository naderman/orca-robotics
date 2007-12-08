SET ( COMP_NAME faithlocaliser )
SET ( SERVICE_NAME OrcaFaithLocaliser )
SET ( OPTION_NAME BUILD_FAITHLOCALISER )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

OPTION( ${OPTION_NAME} "Try to build ${COMP_NAME}" ON )
IF ( BUILD AND NOT ${OPTION_NAME}  )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_LIBRARY( ${COMP_NAME} "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT ${OPTION_NAME} )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} )    
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
