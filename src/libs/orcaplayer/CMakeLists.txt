IF ( NOT WIN32 AND PLAYER_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    
    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/src/libs
        ${PLAYER_INCLUDE_DIRS}
    )
    
    LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
    ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )
    
    GLOBAL_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )

    INSTALL_FILES( /include/orcaplayer .*\\.h$ )
    
    # IF ( BUILD_TESTS )
    #    ADD_SUBDIRECTORY ( test )
    # ENDIF ( BUILD_TESTS )

ELSE  ( NOT WIN32 AND PLAYER_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaPlayer -- Player not found")

ENDIF ( NOT WIN32 AND PLAYER_FOUND )
