IF ( OS_LINUX )

OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
ENDIF( NOT QT4_FOUND )

IF ( BUILD_PATHPLANNER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # "common sources" are common to stand-alone component and IceStorm service 
    # these include ALL .cpp files EXCEPT main.cpp and service.cpp
    # Here, we also need to remove skeletongraphicsI 
    FILE( GLOB COMMON_SRCS *.cpp )
    LIST( REMOVE_ITEM COMMON_SRCS 
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" 
        "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )

    # build common sources into a static "convenience" library
    ADD_LIBRARY ( PathPlannerSources ${COMMON_SRCS} )
    # special dependency on the generated files
    ADD_DEPENDENCIES( PathPlannerSources OrcaInterfaces )

    IF ( QT4_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        ADD_DEFINITIONS( -DQT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner main skeletongraphicsI )
    ELSE( QT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner main  )
    ENDIF( QT4_FOUND )
    
    TARGET_LINK_LIBRARIES( pathplanner PathPlannerSources OrcaOgMap OrcaMisc OrcaPathPlan)

    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED service )
    TARGET_LINK_LIBRARIES( OrcaPathPlanner PathPlannerSources OrcaOgMap OrcaMisc OrcaPathPlan )
    
    GENERATE_CONFIG_FILE( pathplanner.def )

ENDIF ( BUILD_PATHPLANNER )
ENDIF ( OS_LINUX )
