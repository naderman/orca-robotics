ORCA_SET_LIBRARY_NAME( HydroBicyclePlayerClient )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "Only Linux OS is supported" )

include(${ORCA_CMAKE_DIR}/FindPlayer2.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} PLAYER_FOUND "Player not found" )

set( dep_libs HydroInterfaces HydroIceUtil ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_CMAKE_DIR}/UseGearbox.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
    include( ${ORCA_CMAKE_DIR}/UsePlayer.cmake )

    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST( Bicycle bicycleplayerclient driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif( build )
