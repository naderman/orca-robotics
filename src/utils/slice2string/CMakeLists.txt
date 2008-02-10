SET_COMPONENT_NAMES ( Slice2String )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs IceUtil Slice )
    
    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF( build )
