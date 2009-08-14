set( module_name mga_utils )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${module_name} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${module_name} not_os_win "Windows OS is not  supported" )

set( dep_libs GbxUtilAcfr HydroUtil HydroGpsUtil ) 
GBX_REQUIRE_LIBS( build LIB ${module_name} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )

    link_libraries( ${dep_libs} )

    GBX_ADD_EXECUTABLE( latlon2mga latlon2mga.cpp )
    GBX_ADD_EXECUTABLE( mga2latlon mga2latlon.cpp )

    # there is unexplained compile failure in qnx when using ostream
    if( GBX_OS_LINUX )
        GBX_ADD_EXECUTABLE( latlon2mgafile latlon2mgafile.cpp )
    endif( GBX_OS_LINUX )	

endif( build )
