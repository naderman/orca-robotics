set( lib_name ${PROJECT_NAME_CAP}Interfaces )
set( lib_namespace ${PROJECT_NAME_LOWER} )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
# GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

if( build )
    #
    # Defines macros for parsing Slice source files.
    #
    include( ${ORCA_CMAKE_DIR}/Slice2Cpp.cmake )
    
    #
    # All Slice source files are defined in one place (loads ORCA_SLICE_SOURCE_FILES)
    #
    include( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${PROJECT_NAME_LOWER}/slice_sources.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    ORCA_GENERATE_SLICE2CPP_RULES( slice_generated_sources slice_generated_headers ${ORCA_SLICE_SOURCE_FILES} )
    
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
 
    include_directories( ${PROJECT_BINARY_DIR}/src/interfaces/cpp )

    if( NOT ORCA_MOTHERSHIP )
        include( ${ORCA_CMAKE_DIR}/UseOrca.cmake )
    endif( NOT ORCA_MOTHERSHIP )
    
    # IceStorm is not included in UseIce.cmake
    set( dep_libs ${int_libs} ${ext_libs} IceStorm )

    GBX_ADD_LIBRARY( ${lib_name} DEFAULT ${slice_generated_sources} )
    target_link_libraries( ${lib_name} ${dep_libs} )

    # for satellite projects inside a super-project, need to wait until the Orca library is build
    if( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )
        add_dependencies( ${lib_name} OrcaInterfaces )
    endif( NOT ORCA_MOTHERSHIP AND IS_SUPER_PROJECT )
    
    if( ORCA_MOTHERSHIP )    
        # only Orca installs generated headers
        foreach( slice_generated_header ${slice_generated_headers} )
            set( slice_generated_headers_full ${slice_generated_headers_full} ${CMAKE_CURRENT_BINARY_DIR}/${slice_generated_header} )
        endforeach( slice_generated_header )
        
        GBX_ADD_HEADERS( ${lib_namespace} ${slice_generated_headers_full} )
    endif( ORCA_MOTHERSHIP )

endif( build )
