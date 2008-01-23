SET ( LIB_NAME OrcaDef )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )

SET( DEP_LIBS OrcaObjects )
REQUIRE_TARGETS( BUILD LIB ${LIB_NAME} ${DEP_LIBS} )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    IF(NOT OS_WIN)
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    ELSE (NOT OS_WIN)
        # for some reason cannot get shared libs working in windows
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} STATIC ${SRCS} )
    ENDIF (NOT OS_WIN)
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEP_LIBS} )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${LIB_NAME} OrcaObjects )
    
    INSTALL_FILES( /include/orcadef .*\\.h$ )
    
    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD )
