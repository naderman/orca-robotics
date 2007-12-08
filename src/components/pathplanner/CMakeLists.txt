SET ( COMP_NAME pathplanner )
SET ( SERVICE_NAME OrcaPathPlanner )
SET ( OPTION_NAME BUILD_PATHPLANNER )

SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

OPTION( ${OPTION_NAME} "Try to build ${COMP_NAME}" ON )
IF ( BUILD AND NOT ${OPTION_NAME}  )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT ${OPTION_NAME} )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaOgMap  OrcaPathPlan )

    IF ( QT4_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        ADD_DEFINITIONS( -DQT4_FOUND )
    ELSE( QT4_FOUND )
        LIST( REMOVE_ITEM COMP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )
        MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
    ENDIF( QT4_FOUND )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    # build an IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} )
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
