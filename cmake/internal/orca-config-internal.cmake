set( ORCA_FOUND 1 )   

# This is potentially problematic: installed directory structure is different from in-source one.
set( ORCA_INCLUDE_DIR 
    ${CMAKE_SOURCE_DIR}/orca/src/hydrolibs
    ${CMAKE_SOURCE_DIR}/orca/src           # for hydrointerfaces
    ${CMAKE_SOURCE_DIR}/orca/src/hydrodrivers
    ${CMAKE_SOURCE_DIR}/orca/src/libs
    ${CMAKE_BINARY_DIR}/orca/src/interfaces/cpp )

message( STATUS "DEBUG!!! ORCA_INCLUDE_DIR=${ORCA_INCLUDE_DIR}" )

set( ORCA_CMAKE_DIR ${CMAKE_SOURCE_DIR}/orca/cmake )

set( ORCA_USE_FILE ${ORCA_CMAKE_DIR}/orca-use-file.cmake )

set( ORCA_SLICE_DIR ${CMAKE_SOURCE_DIR}/orca/src/interfaces/slice )

# this is where Orca libs will be installed 
set( ORCA_LINK_DIR ${CMAKE_INSTALL_PREFIX}/lib/orca )

set( ORCA_DEFTOOLS_BIN_DIR ${CMAKE_BINARY_DIR}/orca/src/utils/deftools )
set( ORCA_SLICE2ORCA_BIN_DIR ${CMAKE_BINARY_DIR}/orca/src/utils/slice2orca )

# for compatibility with code which uses Hydro only
set( HYDRO_INCLUDE_DIR 
    ${CMAKE_SOURCE_DIR}/orca/src/hydrolibs
    ${CMAKE_SOURCE_DIR}/orca/src           # for hydrointerfaces
    ${CMAKE_SOURCE_DIR}/orca/src/hydrodrivers )

set( HYDRO_LINK_DIR ${CMAKE_INSTALL_PREFIX}/lib/orca )
