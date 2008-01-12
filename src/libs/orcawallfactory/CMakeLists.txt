SET ( LIB_NAME OrcaWallFactory )
GLOBAL_ADD_LICENSE( LGPL )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} OrcaWall OrcaIfaceImpl  )
    
ENDIF ( BUILD )
