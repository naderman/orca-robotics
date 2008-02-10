SET ( APP_NAME pureice )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build EXE ${APP_NAME} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

IF ( build)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )

    FILE( GLOB srcs *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )

ENDIF ( build )
