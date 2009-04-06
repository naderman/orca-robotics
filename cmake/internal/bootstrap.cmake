#
# Figure out if we are in the main Orca project or not
# If it is, all necessary files are in this project.
# Otherwise, we need to find Orca installation directory.
#
# This file should be copied verbatim into [PROJECT]/cmake/local
# directory of satelite projects.
#
include( cmake/internal/Assert.cmake )

if( ${PROJECT_NAME} STREQUAL "orca" )
    message( STATUS "Setting project type to ORCA MOTHERSHIP" )
    set( ORCA_MOTHERSHIP 1 )

else( ${PROJECT_NAME} STREQUAL "orca" )
    message( STATUS "Setting project type to ORCA SATELLITE" )
    set( ORCA_MOTHERSHIP 0 )

endif( ${PROJECT_NAME} STREQUAL "orca" )

#
# Official dependency number 0: Orca
#

if( ORCA_MOTHERSHIP )

    set( ORCA_CMAKE_DIR ${${PROJECT_NAME}_SOURCE_DIR}/cmake )

else( ORCA_MOTHERSHIP )

    # if( DEFINED ORCA_FOUND AND ORCA_FOUND )
    # 
    #     message( STATUS "Orca was previously found - includes are in ${ORCA_INCLUDE_DIR}" )
    # 
    # else( DEFINED ORCA_FOUND AND ORCA_FOUND )
        
        if( DEFINED IS_SUPER_PROJECT )
            
            find_path( super_orca_manifest_dir orca_manifest.cmake ${CMAKE_BINARY_DIR}/orca NO_DEFAULT_PATH )
        
            ASSERT( super_orca_manifest_dir 
                    "Looking for super-project Orca - not found. Is this a super-project?" 
                    "Looking for super-project Orca - found manifest in ${super_orca_manifest_dir}" 
                    1 )
        
            set( ORCA_FOUND 1 CACHE BOOL "Do we have Orca?" FORCE )   
        
            set( ORCA_MANIFEST_DIR ${CMAKE_BINARY_DIR}/orca )
            # This is potentially problematic: installed directory structure is different from in-source one.
            set( ORCA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/orca/src/libs 
                                  ${CMAKE_BINARY_DIR}/orca/src/interfaces/cpp )
            set( ORCA_CMAKE_DIR ${CMAKE_SOURCE_DIR}/orca/cmake )
            set( ORCA_SLICE_DIR ${CMAKE_SOURCE_DIR}/orca/src/interfaces/slice )
            # unnecessary: cmake will find the targets without any directory specified.
        #     set( ORCA_LIB_DIR ${CMAKE_BINARY_DIR}/orca/??? )
            # this is where Orca libs will be installed 
            set( ORCA_LINK_DIR ${CMAKE_INSTALL_PREFIX}/lib/orca )
            set( ORCA_DEFTOOLS_BIN_DIR ${CMAKE_BINARY_DIR}/orca/src/utils/deftools )
            set( ORCA_SLICE2ORCA_BIN_DIR ${CMAKE_BINARY_DIR}/orca/src/utils/slice2orca )
        
        else( DEFINED IS_SUPER_PROJECT )
        
            include( cmake/internal/FindOrca.cmake )
            ASSERT( ORCA_FOUND 
                    "Looking for Orca - not found. Please install Orca, then re-run CMake." 
                    "Looking for Orca - includes found in ${ORCA_INCLUDE_DIR}" 
                    1 )
    
            set( ORCA_MANIFEST_DIR ${ORCA_HOME} )    
            set( ORCA_INCLUDE_DIR ${ORCA_HOME}/include/orca )
            set( ORCA_CMAKE_DIR ${ORCA_HOME}/share/orca/cmake )
            set( ORCA_SLICE_DIR ${ORCA_HOME}/share/orca/slice )
            # this is where Orca libs are installed and to where the *built* Satelite libs will be linked
            set( ORCA_LIB_DIR ${ORCA_HOME}/lib/orca )
            # this is where Orca libs are installed and to where the *installed* Satelite libs will be linked
            set( ORCA_LINK_DIR ${ORCA_HOME}/lib/orca )         
            set( ORCA_DEFTOOLS_BIN_DIR ${ORCA_HOME}/bin )
            set( ORCA_SLICE2ORCA_BIN_DIR ${ORCA_HOME}/bin )

        endif( DEFINED IS_SUPER_PROJECT )
    
    # endif( DEFINED ORCA_FOUND AND ORCA_FOUND )

    #
    # Load Orca manifest
    #
    include( ${ORCA_MANIFEST_DIR}/orca_manifest.cmake )
    ASSERT( ORCA_MANIFEST_LOADED 
            "Loading Orca manifest - failed. Please reinstall Orca, then re-run CMake." 
            "Loading Orca manifest - loaded." 
            1 )

endif( ORCA_MOTHERSHIP )

message( STATUS "Using custom CMake scripts in ${ORCA_CMAKE_DIR}" )

#
# The rest is done by a script common to both Orca and derived projects
#
include( ${ORCA_CMAKE_DIR}/Setup.cmake )
