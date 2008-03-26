# Locate Ice home

# This module defines the following variables:
# ICE_FOUND : 1 if Ice is found, 0 otherwise
# ICE_HOME  : path where to find include, lib, bin, etc.
#
#
# ICEJ_FOUND : 1 if Ice for Java is found, 0 otherwise
# ICEJ_HOME  : path where to find Ice for Java archive

#
# Ice for C++
#

# Assumption: we look for Ice.h and assume that the rest is there.
# i.e. slice2cpp, libIce.so, etc.
# to be more robust we can look for all of those things individually.

# start with 'not found'
SET( ICE_FOUND 0 CACHE BOOL "Do we have Ice?" )

FIND_PATH( ICE_HOME_INCLUDE_ICE Ice.h
  # rational for this search order:
  #    source install w/env.var -> source install
  #    package -> package
  #    package + source install w/env.var -> source install
  #    package + source install w/out env.var -> package 
  #
  # installation selected by user
  ${ICE_HOME}/include/Ice
  $ENV{ICE_HOME}/include/Ice
  # debian package installs Ice here
  /usr/include/Ice
  # Test standard installation points: generic symlinks first, then standard dirs, newer first
  /opt/Ice/include/Ice
  /opt/Ice-3/include/Ice
  /opt/Ice-3.2/include/Ice
  /opt/Ice-3.2.1/include/Ice
  /opt/Ice-3.2.0/include/Ice
  # some people may manually choose to install Ice here
  /usr/local/include/Ice
  # windows
  C:/Ice-3.2.1-VC80/include/Ice
  C:/Ice-3.2.1/include/Ice
  C:/Ice-3.2.0-VC80/include/Ice
  C:/Ice-3.2.0/include/Ice
  )
# MESSAGE( STATUS "DEBUG: Ice.h is apparently found in : ${ICE_HOME_INCLUDE_ICE}" )

# NOTE: if ICE_HOME_INCLUDE_ICE is set to *-NOTFOUND it will evaluate to FALSE
IF( ICE_HOME_INCLUDE_ICE )

    SET( ICE_FOUND 1 CACHE BOOL "Do we have Ice?" FORCE )

    # strip 'file' twice to get rid off 'include/Ice'
#     MESSAGE( STATUS "DEBUG: ICE_HOME_INCLUDE_ICE=" ${ICE_HOME_INCLUDE_ICE} )
    GET_FILENAME_COMPONENT( ICE_HOME_INCLUDE ${ICE_HOME_INCLUDE_ICE} PATH )
    #MESSAGE( STATUS "DEBUG: ICE_HOME_INCLUDE=" ${ICE_HOME_INCLUDE} )
    GET_FILENAME_COMPONENT( ICE_HOME ${ICE_HOME_INCLUDE} PATH CACHE )
    MESSAGE( STATUS "Setting ICE_HOME to ${ICE_HOME}" )

    # debian package splits off slice files into a different place
    IF( ICE_HOME MATCHES /usr )
        SET( ICE_SLICE_HOME /usr/share CACHE PATH "Installation point of standard Slice files" )
        MESSAGE( STATUS "This is a Debian Ice installation. Slice files are in ${ICE_SLICE_HOME}" )
    ELSE ( ICE_HOME MATCHES /usr )
        SET( ICE_SLICE_HOME ${ICE_HOME} CACHE PATH "Installation point of standard Slice files" )
        MESSAGE( STATUS "This is NOT a Debian Ice installation. Slice files are in ${ICE_SLICE_HOME}" )
    ENDIF( ICE_HOME MATCHES /usr )

ENDIF( ICE_HOME_INCLUDE_ICE )

#
# Ice for Java
#

# start with 'not found'
SET( ICEJ_FOUND 0 CACHE BOOL "Do we have Ice for Java?" )

# don't bother if Ice for C++ was not found
IF( ICE_FOUND )

    FIND_PATH( ICEJ_HOME Ice.jar
    # installation selected by user
    $ENV{ICEJ_HOME}
    # debian package installs Ice here
    /usr/share/java
    # maybe user put it into the same place as Ice
    ${ICE_HOME}/lib
    )
    
    IF( ICEJ_HOME )
    
        SET( ICEJ_FOUND 1 CACHE BOOL "Do we have Ice?" FORCE )
        MESSAGE( STATUS "Setting ICEJ_HOME to ${ICEJ_HOME}" )
    
    ENDIF( ICEJ_HOME )

ENDIF( ICE_FOUND )
