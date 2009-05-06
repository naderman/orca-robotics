# Find Orca includes and library.
#
#

set( ORCA_FOUND 1 )

get_filename_component( _found_dir "${CMAKE_CURRENT_LIST_FILE}" PATH )

# load all exported Orca targets
include( ${_found_dir}/orca-targets.cmake )

# assume that orca-config.cmake was installed into
# <install-root>/lib/orca/ 
set( _install_dir "${_found_dir}/../../" )

get_filename_component( 
    ORCA_INCLUDE_DIR 
    "${_install_dir}/include/orca" 
    ABSOLUTE )

get_filename_component( 
    ORCA_CMAKE_DIR 
    "${_install_dir}/share/orca/cmake"
    ABSOLUTE )

get_filename_component( 
    ORCA_USE_FILE 
    "${_install_dir}/share/orca/cmake/orca-use-file.cmake"
    ABSOLUTE )

get_filename_component( 
    ORCA_SLICE_DIR 
    "${_install_dir}/share/orca/slice"
    ABSOLUTE )

get_filename_component( 
    ORCA_LINK_DIR 
    "${_install_dir}/lib/orca" 
    ABSOLUTE )

get_filename_component(
    ORCA_DEFTOOLS_BIN_DIR
    "${_install_dir}/bin"
    ABSOLUTE )

get_filename_component(
    ORCA_SLICE2ORCA_BIN_DIR
    "${_install_dir}/bin"
    ABSOLUTE )

set( _found_dir )
set( _install_dir )
