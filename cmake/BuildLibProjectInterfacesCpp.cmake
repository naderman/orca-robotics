set( _lib_name ${GBX_PROJECT_NAME_CAP}Interfaces )
set( _lib_namespace ${GBX_PROJECT_NAME_LOWER} )
set( _lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
# GBX_REQUIRE_OPTION( build LIB ${_lib_name} ON )

if( build )
    #
    # Defines macros for parsing Slice source files.
    #
    include( ${ORCA_CMAKE_DIR}/Slice2Cpp.cmake )
    
    #
    # All Slice source files are defined in one place (loads ORCA_SLICE_SOURCE_FILES)
    #
    include( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${GBX_PROJECT_NAME_LOWER}/slice_sources.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    ORCA_GENERATE_SLICE2CPP_RULES( _gen_sources _gen_headers ${ORCA_SLICE_SOURCE_FILES} )
    
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
 
    include_directories( ${PROJECT_BINARY_DIR}/src/interfaces/cpp )

    if( NOT ORCA_MOTHERSHIP )
        include( ${ORCA_CMAKE_DIR}/UseOrca.cmake )
    endif( NOT ORCA_MOTHERSHIP )
    
    # IceStorm is not included in UseIce.cmake
    set( _dep_libs IceStorm )

    GBX_ADD_LIBRARY( ${_lib_name} DEFAULT ${_lib_version} ${_gen_sources} )
    target_link_libraries( ${_lib_name} ${_dep_libs} )

    # for satellite projects inside a super-project, need to wait until the Orca library is build
    if( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )
        add_dependencies( ${_lib_name} OrcaInterfaces )
    endif( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )
    
    if( ORCA_MOTHERSHIP )    
        # only Orca installs generated headers
        foreach( _gen_header ${_gen_headers} )
            set( slice_generated_headers_full ${slice_generated_headers_full} ${CMAKE_CURRENT_BINARY_DIR}/${_gen_header} )
        endforeach( _gen_header )
        
        GBX_ADD_HEADERS( ${_lib_namespace} ${slice_generated_headers_full} )
    endif( ORCA_MOTHERSHIP )

endif( build )
