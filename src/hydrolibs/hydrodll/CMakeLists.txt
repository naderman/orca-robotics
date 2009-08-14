ORCA_SET_LIBRARY_NAME( HydroDll )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
        
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )

    if( NOT GBX_OS_WIN )
        target_link_libraries( ${LIB_NAME} dl )
    else( NOT GBX_OS_WIN )
        target_link_libraries( ${LIB_NAME} kernel32 )
    endif( NOT GBX_OS_WIN )
    
    GBX_ADD_HEADERS( hydrodll ${hdrs} )
    
    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )
