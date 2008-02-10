SET_COMPONENT_NAMES ( Teleop )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build EXE ${APP_NAME} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

SET( dep_libs HydroDll HydroInterfaces )
REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
REQUIRE_VAR( build EXE ${APP_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )

    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
