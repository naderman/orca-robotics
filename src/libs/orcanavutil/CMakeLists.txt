IF ( NOT WIN32 )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

    ADD_DEFINITIONS( -Wall )

    LINK_DIRECTORIES(
    )

    INCLUDE_DIRECTORIES(
      ${PROJECT_SOURCE_DIR}/src/libs
    )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaNavUtil SHARED ${SRCS} )

    INSTALL_FILES( /include/orcanavutil .*\\.h$ )
    
    IF ( BUILD_TESTS )
      ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( NOT WIN32 )