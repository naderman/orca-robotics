# Find Hydro includes and library.
#
#

set( HYDRO_FOUND 1 )

get_filename_component( _found_dir "${CMAKE_CURRENT_LIST_FILE}" PATH )

# load all exported Hydro targets
include( ${_found_dir}/hydro-targets.cmake )

# assume that hydro-config.cmake was installed into
# <install-root>/lib/hydro/ 
set( _install_dir "${_found_dir}/../../" )

get_filename_component( 
    HYDRO_INCLUDE_DIR 
    "${_install_dir}/include/hydro" 
    ABSOLUTE )

get_filename_component( 
    HYDRO_CMAKE_DIR 
    "${_install_dir}/share/hydro/cmake"
    ABSOLUTE )

get_filename_component( 
    HYDRO_USE_FILE 
    "${_install_dir}/share/hydro/cmake/hydro-use-file.cmake"
    ABSOLUTE )

get_filename_component( 
    HYDRO_LINK_DIR 
    "${_install_dir}/lib/hydro" 
    ABSOLUTE )

set( _found_dir )
set( _install_dir )
