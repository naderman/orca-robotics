IF ( NOT OS_WIN )

    # alexm: bloody  needs to be reviewed!
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaGpsUtil SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaGpsUtil  )
    INSTALL_FILES( /include/orcagpsutil .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( NOT OS_WIN )