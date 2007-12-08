SET ( COMP_NAME teleop )
SET ( SERVICE_NAME OrcaFaithLocaliser )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS HydroDll HydroInterfaces )

    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
