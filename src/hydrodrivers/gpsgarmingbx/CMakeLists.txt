ORCA_SET_LIBRARY_NAME( HydroGpsGarminGbx )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs GbxUtilAcfr GbxSerialAcfr GbxGarminAcfr HydroInterfaces )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${HYDRO_CMAKE_DIR}/UseIceUtil.cmake )
    
    # Source files
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST_WITH_CONFIG( Gps gpsgarmingbx driver.h ${LIB_NAME} )
    endif( ORCA_BUILD_TESTS )

endif( build )
