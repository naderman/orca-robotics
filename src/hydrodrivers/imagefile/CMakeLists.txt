ORCA_SET_LIBRARY_NAME( HydroImageFile )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs HydroIceUtil HydroInterfaces HydroImage ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

if( build )

    include( ${GEARBOX_USE_FILE} )
    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${HYDRO_CMAKE_DIR}/UseIceUtil.cmake )
    include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    # Source files
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST_WITH_CONFIG( Image imagefile driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif( build )
