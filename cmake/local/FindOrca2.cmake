# Locate Orca2 home

# This module defines
# ORCA2_HOME where to find include, lib, bin, etc.
# ORCA2_FOUND, If set to 0, don't try to use Orca2.

# start with 'not found'
SET( ORCA2_FOUND 0 CACHE BOOL "Do we have Orca2?" )

FIND_PATH( ORCA2_HOME_INCLUDE_ORCA orca.h
  $ENV{ORCA2_HOME}/include/orca
  # Test standard installation points
  /opt/orca2/include/orca
  /opt/orca-2.0.2/include/orca
  /opt/orca-2.0.1/include/orca
  /opt/orca-2.0.0/include/orca
  C:/orca2/include/orca
  )
# MESSAGE( STATUS "DEBUG: orca.h is apparently found in : ${ORCA2_HOME_INCLUDE_ORCA}" )

# NOTE: if ORCA2_HOME_INCLUDE_ORCA is set to *-NOTFOUND it will evaluate to FALSE
IF ( ORCA2_HOME_INCLUDE_ORCA )

    SET( ORCA2_FOUND 1 CACHE BOOL "Do we have Orca2?" FORCE )

    # strip 'file' twice to get rid off 'include/orca'
#     MESSAGE( STATUS "DEBUG: ORCA2_HOME_INCLUDE_ORCA=" ${ORCA2_HOME_INCLUDE_ORCA} )
    GET_FILENAME_COMPONENT( ORCA2_HOME_INCLUDE ${ORCA2_HOME_INCLUDE_ORCA} PATH )
    #MESSAGE( STATUS "DEBUG: ORCA2_HOME_INCLUDE=" ${ORCA2_HOME_INCLUDE} )
    GET_FILENAME_COMPONENT( ORCA2_HOME ${ORCA2_HOME_INCLUDE} PATH )
    #MESSAGE( STATUS "DEBUG: ORCA2_HOME=" ${ORCA2_HOME} )
ENDIF ( ORCA2_HOME_INCLUDE_ORCA )

# do we need it in cache?
# SET( ORCA2_HOME ${ORCA2_HOME} CACHE PATH "Orca2 installed directory" FORCE )
#   MESSAGE( "Setting Orca2 installed directory to ${ORCA2_HOME}" )
