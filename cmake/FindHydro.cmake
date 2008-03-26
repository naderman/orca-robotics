# Locate Hydro installation

# This module defines
# HYDRO_HOME where to find include, lib, bin, etc.
# HYDRO_FOUND, If set to 0, don't try to use Hydro libraries, scripts, etc.

# start with 'not found'
SET( HYDRO_FOUND 0 CACHE BOOL "Do we have Hydro?" )

# this will work in Orca which has the same version as Hydro
IF( ORCA_MOTHERSHIP )
    SET( HYDRO_SEARCH_DIR_ORCA_ONLY /opt/hydro-${PROJECT_VERSION} )
ENDIF( ORCA_MOTHERSHIP )

FIND_PATH( HYDRO_HOME hydro_manifest.cmake
    # Test user-specified installation point (first look in the dir specified
    # with command line CMake variable, then with environment variable)
    ${HYDRO_HOME}
    $ENV{HYDRO_HOME}
    # Test common installation points
    /usr/local
    /opt/hydro
    ${HYDRO_SEARCH_DIR_ORCA_ONLY}
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
# MESSAGE( STATUS "DEBUG: manifest.cmake is apparently found in : ${HYDRO_HOME}" )

# NOTE: if HYDRO_HOME is set to *-NOTFOUND it will evaluate to FALSE
IF( HYDRO_HOME )
    SET( HYDRO_FOUND 1 CACHE BOOL "Do we have Hydro?" FORCE )
ENDIF( HYDRO_HOME )
