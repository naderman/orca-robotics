IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaNavUtil SHARED ${SRCS} )

    INSTALL_FILES( /include/orcanavutil .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( NOT OS_WIN )