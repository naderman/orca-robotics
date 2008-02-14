# Locate Hydro installation

# This module defines
# HYDRO_HOME where to find include, lib, bin, etc.
# HYDRO_FOUND, If set to 0, don't try to use Hydro libraries, scripts, etc.

# start with 'not found'
SET( HYDRO_FOUND 0 CACHE BOOL "Do we have Hydro?" )

FIND_PATH( HYDRO_HOME hydro_manifest.cmake
  $ENV{HYDRO_HOME}
  # Test standard installation points
  /opt/hydro
  /opt/hydro-${PROJECT_VERSION}   # this will work in Orca which has the same version as Hydro
  /opt/hydro-2.15.0
  /opt/hydro-2.14.0
  /opt/hydro-2.13.0
  /opt/hydro-2.12.0
  /opt/hydro-2.11.0
  /opt/hydro-2.10.0
  /opt/hydro-2.9.0
  /opt/hydro-2.8.0
  /opt/hydro-2.7.0
  /opt/hydro-2.6.0
  C:/hydro
  )
# MESSAGE( STATUS "DEBUG: manifest.cmake is apparently found in : ${HYDRO_HOME}" )

# NOTE: if HYDRO_HOME is set to *-NOTFOUND it will evaluate to FALSE
IF ( HYDRO_HOME )
    SET( HYDRO_FOUND 1 CACHE BOOL "Do we have Hydro?" FORCE )
ENDIF ( HYDRO_HOME )

# do we need it in cache?
# SET( HYDRO_HOME ${HYDRO_HOME} CACHE PATH "Hydro installed directory" FORCE )
#   MESSAGE( "Setting Hydro installed directory to ${HYDRO_HOME}" )
