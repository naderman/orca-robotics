set( HYDRO_FOUND 1 )   

# This is potentially problematic: installed directory structure is different from in-source one.
set( HYDRO_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/hydro/src/libs"
                    "${CMAKE_SOURCE_DIR}/hydro/src/hydrodrivers" 
                    "${CMAKE_SOURCE_DIR}/hydro/src" )

set( HYDRO_CMAKE_DIR "${CMAKE_SOURCE_DIR}/hydro/cmake" )

set( HYDRO_USE_FILE "${HYDRO_CMAKE_DIR}/hydro-use-file.cmake" )

# this is where Hydro libs will be installed 
set( HYDRO_LINK_DIR ${CMAKE_INSTALL_PREFIX}/lib/hydro )
