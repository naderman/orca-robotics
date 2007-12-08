SET ( COMP_NAME pureice )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${SRCS} )

ENDIF ( BUILD )
