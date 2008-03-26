MESSAGE( STATUS "Planning to build C++ version of Slice interfaces." )

#
# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the namespace, e.g. 'orca'
#
GET_FILENAME_COMPONENT( slice_module ${CMAKE_CURRENT_SOURCE_DIR} NAME )

#
# Defines macros for compiling Slice source files.
#
INCLUDE( ${ORCA_CMAKE_DIR}/slice2cpp_rules.cmake )

#
# All Slice source files are defined in one place (loads ORCA_SLICE_SOURCE_FILES)
#
INCLUDE( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${slice_module}/slice_sources.cmake )

#
# Work out the list of generated files from the list of slice sources
# The list of headers is used again in UseLibProjectInterfaces.cmake
#
GENERATE_SLICE2CPP_RULES( slice_generated_sources slice_generated_headers ${ORCA_SLICE_SOURCE_FILES} )
# MESSAGE( STATUS "DEBUG: build_interface_lib_cpp.cmake: slice_generated_headers=${slice_generated_headers}" )
SET( ORCA_SLICE_GENERATED_HEADERS ${slice_generated_headers} CACHE INTERNAL "C++ source files generated from Slice sources" FORCE )


INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )

INCLUDE_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/${slice_module}
)

# for satelite projects, add the orca install directory
IF( DEFINED ORCA_HOME )
    INCLUDE_DIRECTORIES( ${ORCA_HOME}/include/orca )
ENDIF( DEFINED ORCA_HOME )

IF( NOT GBX_OS_WIN )
    GBX_ADD_LIBRARY( ${PROJECT_INTERFACE_LIB} SHARED ${slice_generated_sources} )
ELSE( NOT GBX_OS_WIN )
    GBX_ADD_LIBRARY( ${PROJECT_INTERFACE_LIB} STATIC ${slice_generated_sources} )
ENDIF( NOT GBX_OS_WIN ) 

FOREACH( slice_generated_header ${slice_generated_headers} )
    SET( slice_generated_headers_full ${slice_generated_headers_full} ${CMAKE_CURRENT_BINARY_DIR}/${slice_generated_header} )
ENDFOREACH( slice_generated_header )

GBX_ADD_HEADERS( ${slice_module} ${slice_generated_headers_full} )
