# Locate Hydro installation

# This module defines
# HYDRO_FOUND, If set to 0, don't try to use Hydro libraries, scripts, etc.

# start with 'not found'
set( HYDRO_FOUND 0 CACHE BOOL "Do we have Hydro?" )

# this will work in Orca which has the same version as Hydro
if( ORCA_MOTHERSHIP )
    set( hydro_search_dir_orca_only /opt/hydro-${PROJECT_VERSION} )
endif( ORCA_MOTHERSHIP )

find_path( HYDRO_HOME hydro_manifest.cmake
    # check cmake variable: i.e. specified on the command line
    ${HYDRO_HOME}
    # check environment variable
    $ENV{HYDRO_HOME}
    # check common installation points
    /usr/local
    /opt/hydro
    ${hydro_search_dir_orca_only}
    /opt/hydro-2.15.0+
    /opt/hydro-2.15.0
    /opt/hydro-2.14.0+
    /opt/hydro-2.14.0
    /opt/hydro-2.13.0+
    /opt/hydro-2.13.0
    /opt/hydro-2.12.0+
    /opt/hydro-2.12.0
    /opt/hydro-2.11.0+
    /opt/hydro-2.11.0
    /opt/hydro-2.10.0+
    /opt/hydro-2.10.0
    /opt/hydro-2.9.0+
    /opt/hydro-2.9.0
    /opt/hydro-2.8.0
    /opt/hydro-2.8.0
    /opt/hydro-2.7.0
    /opt/hydro-2.6.0
    C:/hydro
    C:/hydro-${PROJECT_VERSION}
)

if( HYDRO_HOME )    
#     message( STATUS "DEBUG: we found an installed version of hydro at: ${HYDRO_HOME}" )
    set( HYDRO_FOUND 1 CACHE BOOL "Do we have Hydro?" FORCE )   
endif( HYDRO_HOME ) 
