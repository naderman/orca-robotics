ORCA_SET_LIBRARY_NAME( OrcaLogFactory )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )

set( dep_libs OrcaLog OrcaIfaceImpl OrcaObjects OrcaObjectUtil OrcaIfaceLog )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # for jpeg camera logging
    include( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
    if( OPENCV_FOUND )
        INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)
        add_definitions( -DOPENCV_FOUND )
    endif( OPENCV_FOUND )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )

    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )
    
endif( build )
