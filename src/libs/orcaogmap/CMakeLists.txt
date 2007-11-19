IF ( NOT OS_WIN) 

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaOgMap SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaOgMap HydroOgMap )

    INSTALL_FILES( /include/orcaogmap .*\\.h$ )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( NOT OS_WIN) 
