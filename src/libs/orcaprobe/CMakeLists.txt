SET ( LIB_NAME OrcaProbe )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    
    INSTALL_FILES( /include/orcaprobe .*\\.h$ ) 

ENDIF ( BUILD )
