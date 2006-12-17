IF ( NOT OS_WIN )

OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
ENDIF( NOT QT4_FOUND )

IF ( BUILD_PATHPLANNER )

    # automatically generate #ifdef's for component.cpp
   CONFIGURE_FILE( configpathplanner.h.cmake ${PROJECT_SOURCE_DIR}/src/components/pathplanner/configpathplanner.h )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
            algohandler
            astardriver
            component
            fakedriver
            simplenavdriver
            skeletondriver
            pathplanner2dI
    )
    
    SET ( QT_SRCS skeletongraphicsI )

    IF ( QT4_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner main ${COMMON_SRCS} ${QT_SRCS} )
        # build an IceBox service
        GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED service ${COMMON_SRCS} )
        TARGET_LINK_LIBRARIES( pathplanner OrcaOgMap OrcaPathPlan OrcaMisc )
    ELSE( QT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner main ${COMMON_SRCS} )
        # build an IceBox service
        GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED service ${COMMON_SRCS} )
        TARGET_LINK_LIBRARIES( pathplanner OrcaOgMap OrcaPathPlan OrcaMisc )
    ENDIF( QT4_FOUND )
    
    
    GENERATE_CONFIG_FILE( pathplanner.def )

ENDIF ( BUILD_PATHPLANNER )
ENDIF ( NOT OS_WIN )
