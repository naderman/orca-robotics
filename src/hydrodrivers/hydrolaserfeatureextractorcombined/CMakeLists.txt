ORCA_SET_LIBRARY_NAME( HydroLaserFeatureExtractorCombined )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs GbxUtilAcfr GbxIceUtilAcfr HydroLaserFeatureExtract )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    # install the headers so others can re-use parts
    file( GLOB hdrs *.h )
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    # Test for correct linking.
    if( ORCA_BUILD_TESTS )
        # the constructor of this driver has a non-standard signature:
        # it uses a config file from some library, not the one defined in its interface.
        # because of this we cannot use a standard linktest macro
#         HYDRO_ADD_LINK_TEST( LaserFeatureExtractor lfecombined driver.h ${LIB_NAME} ${dep_libs} )
        list( REMOVE_ITEM srcs "${CMAKE_CURRENT_SOURCE_DIR}/linktest.cpp" )
        add_executable( linktest linktest.cpp )
        target_link_libraries( linktest ${LIB_NAME} )
    endif( ORCA_BUILD_TESTS )

endif( build )
