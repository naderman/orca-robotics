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
        
    if( DEFINED IS_SUPER_PROJECT )
        
        set( o_config_file orca-config-internal.cmake )
        set( o_search_dir ${CMAKE_SOURCE_DIR}/orca/cmake/internal )
        find_path( o_found_dir ${o_config_file} PATHS ${o_search_dir} NO_DEFAULT_PATH )
    
        ORCA_ASSERT( o_found_dir 
                "Looking for super-project Orca - not found. Is this a super-project?" 
                "Looking for super-project Orca - found internal config in ${o_found_dir}" 
                1 )
    
        include( ${o_found_dir}/${o_config_file} )

    else( DEFINED IS_SUPER_PROJECT )

        set( orca_search_path
            # Test user-specified installation point (first look in the dir specified
            # with command line CMake variable, then with environment variable)
            ${ORCA_HOME}
            $ENV{ORCA_HOME}
            # Test common installation points
            /usr/local
            /opt/orca
            "C:/Program Files/orca/Include"
            C:/orca )
        message( STATUS "Searching for Orca in: ${orca_search_path}" )
                 
        find_package( orca REQUIRED 
            PATHS ${orca_search_path}
            NO_DEFAULT_PATH )

        ORCA_ASSERT( ORCA_FOUND 
                "Looking for Orca - not found." 
                "Looking for Orca - found package config in ${ORCA_LINK_DIR}" 
                1 )

    endif( DEFINED IS_SUPER_PROJECT )


endif( ORCA_MOTHERSHIP )

message( STATUS "Using custom CMake scripts in ${ORCA_CMAKE_DIR}" )

#
# The rest is done by a script common to both Orca and derived projects
#
include( ${ORCA_CMAKE_DIR}/CommonSetup.cmake )
