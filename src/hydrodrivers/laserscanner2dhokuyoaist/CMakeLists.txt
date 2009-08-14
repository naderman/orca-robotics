ORCA_SET_LIBRARY_NAME( HydroLaserScanner2dHokuyoAist )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "Only Linux OS is supported" )

set( dep_libs hokuyo_aist HydroInterfaces ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${HYDRO_CMAKE_DIR}/UseIceUtil.cmake )

    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST_WITH_CONFIG( LaserScanner2d laserscanner2dhokuyoaist driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif(build )
