ORCA_SET_LIBRARY_NAME( HydroLaserFeatureExtract )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs GbxUtilAcfr HydroFeatureObs HydroUtil HydroGeom2d HydroScanUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( hydrolaserfeatureextract ${hdrs} )

    if( ORCA_BUILD_TESTS )
      add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )       
    
endif( build )
