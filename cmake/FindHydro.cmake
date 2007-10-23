# Locate Hydro installation

# This module defines
# HYDRO_HOME where to find include, lib, bin, etc.
# HYDRO_FOUND, If set to 0, don't try to use Hydro libraries, scripts, etc.

# start with 'not found'
SET( HYDRO_FOUND 0 CACHE BOOL "Do we have Hydro?" )

FIND_PATH( HYDRO_HOME_INCLUDE_HYDROUTIL buffer.h
  $ENV{HYDRO_HOME}/include/hydroutil
  # Test standard installation points
  /opt/hydro/include/hydroutil
  /opt/hydro-2.7.0/include/hydroutil
  /opt/hydro-2.6.0+/include/hydroutil
  /opt/hydro-2.6.0/include/hydroutil
  C:/hydro/include/hydroutil
  )
# MESSAGE( STATUS "DEBUG: buffer.h is apparently found in : ${HYDRO_HOME_INCLUDE_HYDROUTIL}" )

# NOTE: if HYDRO_HOME_INCLUDE_HYDROUTIL is set to *-NOTFOUND it will evaluate to FALSE
IF ( HYDRO_HOME_INCLUDE_HYDROUTIL )

    SET( HYDRO_FOUND 1 CACHE BOOL "Do we have Orca?" FORCE )

    # strip 'file' twice to get rid off 'include/hydroutil'
#     MESSAGE( STATUS "DEBUG: HYDRO_HOME_INCLUDE_HYDROUTIL=" ${HYDRO_HOME_INCLUDE_HYDROUTIL} )
    GET_FILENAME_COMPONENT( HYDRO_HOME_INCLUDE ${HYDRO_HOME_INCLUDE_HYDROUTIL} PATH )
#     MESSAGE( STATUS "DEBUG: HYDRO_HOME_INCLUDE=" ${HYDRO_HOME_INCLUDE} )
    GET_FILENAME_COMPONENT( HYDRO_HOME ${HYDRO_HOME_INCLUDE} PATH )
#     MESSAGE( STATUS "DEBUG: HYDRO_HOME=" ${HYDRO_HOME} )

ENDIF ( HYDRO_HOME_INCLUDE_HYDROUTIL )

# do we need it in cache?
# SET( HYDRO_HOME ${HYDRO_HOME} CACHE PATH "Hydro installed directory" FORCE )
#   MESSAGE( "Setting Hydro installed directory to ${HYDRO_HOME}" )
