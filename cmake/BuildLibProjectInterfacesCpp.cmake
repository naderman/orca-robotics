SET( lib_name ${PROJECT_NAME_CAP}Interfaces )
SET( lib_namespace ${PROJECT_NAME_LOWER} )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
# GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

IF( build )
    #
    # Defines macros for parsing Slice source files.
    #
    INCLUDE( ${ORCA_CMAKE_DIR}/Slice2Cpp.cmake )
    
    #
    # All Slice source files are defined in one place (loads ORCA_SLICE_SOURCE_FILES)
    #
    INCLUDE( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${PROJECT_NAME_LOWER}/slice_sources.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    ORCA_GENERATE_SLICE2CPP_RULES( slice_generated_sources slice_generated_headers ${ORCA_SLICE_SOURCE_FILES} )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
 
    INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR}/src/interfaces/cpp )

    IF( NOT ORCA_MOTHERSHIP )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseOrca.cmake )
    ENDIF( NOT ORCA_MOTHERSHIP )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${slice_generated_sources} )
    
    IF( ORCA_MOTHERSHIP )    
        # only Orca installs generated headers
        FOREACH( slice_generated_header ${slice_generated_headers} )
            SET( slice_generated_headers_full ${slice_generated_headers_full} ${CMAKE_CURRENT_BINARY_DIR}/${slice_generated_header} )
        ENDFOREACH( slice_generated_header )
        
        GBX_ADD_HEADERS( ${lib_namespace} ${slice_generated_headers_full} )
    ENDIF( ORCA_MOTHERSHIP )

ENDIF( build )
