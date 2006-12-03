IF ( NOT WIN32 )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

    ADD_DEFINITIONS( -Wall )

    LINK_LIBRARIES( OrcaOgMap OrcaMisc )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaPathPlan SHARED ${SRCS} )

    INSTALL_FILES( /include/orcapathplan .*\\.h$ )
    
    IF( BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( BUILD_TESTS )       

ENDIF ( NOT WIN32 )