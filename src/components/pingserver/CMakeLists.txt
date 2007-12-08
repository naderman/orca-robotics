SET ( COMP_NAME pingserver )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
REQUIRE_VAR( EXE ${COMP_NAME} OS_LINUX "only Linux OS is supported" )
# Look for liboping -- library for 'ping'ing from C
INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )
REQUIRE_VAR( EXE ${COMP_NAME} LIBOPING_FOUND "liboping not found" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDROPING_INSTALLED "libHydroPing not installed" )

IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} HydroPing )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
