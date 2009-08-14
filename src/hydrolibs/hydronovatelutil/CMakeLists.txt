ORCA_SET_LIBRARY_NAME( HydroNovatelUtil )
GBX_ADD_LICENSE( "LGPL (with permission from NovAtel Inc.)" )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    
    # to include header files from subdirectories
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs cmd/*.cpp crc/*.cpp log/*.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    
    GBX_ADD_HEADERS( hydronovatelutil ${hdrs} )
    
    # installation only
    add_subdirectory( cmd )
    add_subdirectory( log )
    add_subdirectory( crc )

endif( build )
