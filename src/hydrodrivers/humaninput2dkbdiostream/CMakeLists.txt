ORCA_SET_LIBRARY_NAME( HydroHumanInput2dKbdIostream )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs GbxUtilAcfr HydroUtil HydroInterfaces )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    
    # Source files
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME}  ${dep_libs} )

endif( build )
