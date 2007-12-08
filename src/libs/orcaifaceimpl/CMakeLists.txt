SET ( LIB_NAME OrcaIfaceImpl )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    IF ( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    ELSE ( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} STATIC ${SRCS} )
    ENDIF ( NOT OS_WIN )
    
    INSTALL_FILES( /include/orcaifaceimpl .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD )