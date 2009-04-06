set( lib_name ${PROJECT_NAME_CAP}IfaceUtil )
set( lib_namespace ${PROJECT_NAME_LOWER}ifaceutil )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

# using Exception
set( ext_libs GbxUtilAcfr )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

if( build )
    #
    # Defines macros for parsing Slice source files.
    #
    include( ${ORCA_CMAKE_DIR}/Slice2Util.cmake )
    
    #
    # All Slice source files are defined in one place (loads ORCA_SLICE_SOURCE_FILES)
    #
    include( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${PROJECT_NAME_LOWER}/slice_sources.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    ORCA_GENERATE_SLICE2UTIL_RULES( slice_generated_sources slice_generated_headers ${ORCA_SLICE_SOURCE_FILES} )
    
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_CMAKE_DIR}/UseGearbox.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    
    if( NOT ORCA_MOTHERSHIP )
        include( ${ORCA_CMAKE_DIR}/UseOrca.cmake )
    endif( NOT ORCA_MOTHERSHIP )

    # add templated utility files
    set( util_h_file ${CMAKE_CURRENT_BINARY_DIR}/util.h )
    set( util_cpp_file ${CMAKE_CURRENT_BINARY_DIR}/util.cpp )

    configure_file( ${ORCA_CMAKE_DIR}/ifaceutilutil.h.template ${util_h_file} )
    configure_file( ${ORCA_CMAKE_DIR}/ifaceutilutil.cpp.template ${util_cpp_file} )

    set( dep_libs ${int_libs} ${ext_libs} )
    
    GBX_ADD_LIBRARY( ${lib_name} DEFAULT ${slice_generated_sources} ${util_cpp_file} )
    target_link_libraries( ${lib_name} ${dep_libs} )
    
    # add dependency on the generator executable when compiling Orca (satellite projects use installed version)
    # OR
    # when everything is built from one super-project.
    if( ORCA_MOTHERSHIP OR IS_SUPER_PROJECT )
        add_dependencies( ${lib_name} slice2orca )
    endif( ORCA_MOTHERSHIP OR IS_SUPER_PROJECT )
    
    # for satellite projects inside a super-project, need to wait until the Orca library is build
    if( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )
        add_dependencies( ${lib_name} OrcaIfaceUtil )
    endif( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )

    if( ORCA_MOTHERSHIP )
        # only Orca installs generated headers
        foreach( slice_generated_header ${slice_generated_headers} )
            set( slice_generated_headers_full ${slice_generated_headers_full} ${CMAKE_CURRENT_BINARY_DIR}/${slice_generated_header} )
        endforeach( slice_generated_header )
        
        GBX_ADD_HEADERS( ${lib_namespace} ${slice_generated_headers_full} ${util_h_file} )

        if( ORCA_BUILD_TESTS )
            add_subdirectory( test )
        endif( ORCA_BUILD_TESTS )
    endif( ORCA_MOTHERSHIP )

endif( build )
