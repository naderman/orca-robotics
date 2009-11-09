ORCA_SET_LIBRARY_NAME( HydroImageCvCamera )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs HydroIceUtil HydroInterfaces HydroImage )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

include(${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} OPENCV_FOUND "OpenCV not found" )

include(${ORCA_CMAKE_DIR}/FindDc1394.cmake)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} DC1394_FOUND "libdc1394v2 (dc1394 ver 2) was not found" )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
    INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        HYDRO_ADD_LINK_TEST_WITH_CONFIG( Image imagecvcamera driver.h ${LIB_NAME} ${dep_libs} )
    endif( ORCA_BUILD_TESTS )

endif( build )
