IF ( OS_LINUX )

OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( BUILD_PATHPLANNER )

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
    GLOBAL_ADD_EXECUTABLE( pathplanner ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( pathplanner ${COMP_DEPS} )

    # build an IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaPathPlanner ${COMP_DEPS} )
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( pathplanner.def )

ENDIF ( BUILD_PATHPLANNER )
ENDIF ( OS_LINUX )
