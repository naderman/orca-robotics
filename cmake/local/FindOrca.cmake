# Locate Orca home

# This module defines
# ORCA_HOME where to find include, lib, bin, etc.
# ORCA_FOUND, If set to 0, don't try to use Orca.

# start with 'not found'
SET( ORCA_FOUND 0 CACHE BOOL "Do we have Orca?" )

FIND_PATH( ORCA_HOME_INCLUDE_ORCA orca.h
  $ENV{ORCA_HOME}/include/orca
  # Test standard installation points
  /opt/orca/include/orca
  /opt/orca-2.7.0/include/orca
  /opt/orca-2.6.0/include/orca
  /opt/orca-2.5.0/include/orca
  /opt/orca-2.4.0/include/orca
  /opt/orca-2.3.0/include/orca
  /opt/orca-2.2.0/include/orca
  /opt/orca-2.2.0/include/orca
  /opt/orca-2.1.0/include/orca
  /opt/orca-2.0.0/include/orca
  C:/orca/include/orca
  )
# MESSAGE( STATUS "DEBUG: orca.h is apparently found in : ${ORCA_HOME_INCLUDE_ORCA}" )

# NOTE: if ORCA_HOME_INCLUDE_ORCA is set to *-NOTFOUND it will evaluate to FALSE
IF ( ORCA_HOME_INCLUDE_ORCA )

    SET( ORCA_FOUND 1 CACHE BOOL "Do we have Orca?" FORCE )

    # strip 'file' twice to get rid off 'include/orca'
#     MESSAGE( STATUS "DEBUG: ORCA_HOME_INCLUDE_ORCA=" ${ORCA_HOME_INCLUDE_ORCA} )
    GET_FILENAME_COMPONENT( ORCA_HOME_INCLUDE ${ORCA_HOME_INCLUDE_ORCA} PATH )
    #MESSAGE( STATUS "DEBUG: ORCA_HOME_INCLUDE=" ${ORCA_HOME_INCLUDE} )
    GET_FILENAME_COMPONENT( ORCA_HOME ${ORCA_HOME_INCLUDE} PATH )
    #MESSAGE( STATUS "DEBUG: ORCA_HOME=" ${ORCA_HOME} )
ENDIF ( ORCA_HOME_INCLUDE_ORCA )

# do we need it in cache?
# SET( ORCA_HOME ${ORCA_HOME} CACHE PATH "Orca installed directory" FORCE )
#   MESSAGE( "Setting Orca installed directory to ${ORCA_HOME}" )
