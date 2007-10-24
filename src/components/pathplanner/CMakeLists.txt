IF ( OS_LINUX )

OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
ENDIF( NOT QT4_FOUND )

IF ( BUILD_PATHPLANNER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # having skeletongraphics makes this comp special, can't call FIND_COMPONENT_SOURCES()
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    # Here, we also need to remove skeletongraphicsI 
    FILE( GLOB COMP_SRCS *.cpp )
    LIST( REMOVE_ITEM COMP_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )

    SET( COMP_DEPS OrcaOgMap OrcaMisc OrcaPathPlan )

    IF ( QT4_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        ADD_DEFINITIONS( -DQT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner ${COMP_SRCS} main skeletongraphicsI )
    ELSE( QT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner ${COMP_SRCS} main  )
    ENDIF( QT4_FOUND )
    
    TARGET_LINK_LIBRARIES( pathplanner ${COMP_DEPS} )

    # build an IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaPathPlanner ${COMP_DEPS} )
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( pathplanner.def )

ENDIF ( BUILD_PATHPLANNER )
ENDIF ( OS_LINUX )
