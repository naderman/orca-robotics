SET ( COMP_NAME orcaview3d )

REQUIRE_OPTION( EXE ${COMP_NAME} OFF )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${COMP_NAME} QT4_FOUND "Qt4 not found" )
REQUIRE_VAR( EXE ${COMP_NAME} GLUT_FOUND "libGlut not found" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${HUMAN_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} OrcaQCompModel OrcaQGui OrcaQGui3d HydroDll )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

    IF ( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
    ENDIF ( LIB3DS_FOUND )

ENDIF ( BUILD )