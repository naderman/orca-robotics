# Find Orca includes and library.
#
#

set( ORCA_FOUND 1 )

get_filename_component( _found_dir "${CMAKE_CURRENT_LIST_FILE}" PATH )
include( ${_found_dir}/orca-targets.cmake )

get_filename_component( 
    ORCA_INCLUDE_DIR 
    "${_found_dir}/../../include/orca" 
    ABSOLUTE )

get_filename_component( 
    ORCA_CMAKE_DIR 
    "${_found_dir}/../../share/orca/cmake"
    ABSOLUTE )

get_filename_component( 
    ORCA_USE_FILE 
    "${_found_dir}/../../share/orca/cmake/orca-use-file.cmake"
    ABSOLUTE )

get_filename_component( 
    ORCA_SLICE_DIR 
    "${_found_dir}/../../share/orca/slice"
    ABSOLUTE )

get_filename_component( 
    ORCA_LINK_DIR 
    "${_found_dir}/../../lib/orca" 
    ABSOLUTE )

get_filename_component(
    ORCA_DEFTOOLS_BIN_DIR
    "${_found_dir}/../../bin"
    ABSOLUTE )

get_filename_component(
    ORCA_SLICE2ORCA_BIN_DIR
    "${_found_dir}/../../bin"
    ABSOLUTE )

set( _found_dir )
