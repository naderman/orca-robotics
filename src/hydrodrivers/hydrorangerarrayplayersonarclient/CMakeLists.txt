ORCA_SET_LIBRARY_NAME( HydroRangerArrayPlayerSonarClient )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "Only Linux OS is supported" )

include(${ORCA_CMAKE_DIR}/FindPlayer2.1.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} PLAYER_FOUND "Player not found" )

set( dep_libs GbxUtilAcfr HydroUtil HydroInterfaces HydroIceUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UsePlayer.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST( RangerArray rangerarrayplayersonarclient driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif( build )
