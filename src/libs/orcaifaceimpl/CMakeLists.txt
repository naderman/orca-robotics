SET ( LIB_NAME OrcaIfaceImpl )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    IF ( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    ELSE ( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} STATIC ${SRCS} )
    ENDIF ( NOT OS_WIN )

    TARGET_LINK_LIBRARIES( ${LIB_NAME} OrcaObjects )
    
    INSTALL_FILES( /include/orcaifaceimpl .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD )