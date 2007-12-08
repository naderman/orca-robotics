SET ( COMP_NAME logplayer )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} OrcaLog HydroNavUtil HydroDll )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )
    
ENDIF( BUILD )
