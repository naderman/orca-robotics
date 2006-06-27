OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( NOT WIN32 )
# Needs Qt4 for displaying debug stuff
IF ( BUILD_PATHPLANNER AND QT4_FOUND )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
            algohandler
            astardriver
            component
            fakedriver
            skeletongraphicsI
            pathplanner2dI
            gridpotentialdriver
    )

    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${PROJ_SOURCE_DIR}/src/utils
    )

    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
   
    # Files for the component
    SET( COMP_SRCS main  ${COMMON_SRCS} )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pathplanner ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( pathplanner OrcaOgMap OrcaPathPlan OrcaMisc  ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
    
    # Files for the service
    SET( SERV_SRCS  service ${COMMON_SRCS} )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaPathPlanner SHARED ${SERV_SRCS} )
    
    INSTALL_TARGETS( /lib OrcaPathPlanner )

    GENERATE_FROM_DEF( pathplanner.def )

ELSE ( BUILD_PATHPLANNER AND QT4_FOUND )

    MESSAGE("Not Planning to Build Pathplanner -- Qt4 not found")

ENDIF ( BUILD_PATHPLANNER AND QT4_FOUND )
ENDIF ( NOT WIN32 )
