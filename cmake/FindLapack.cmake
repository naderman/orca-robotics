# Confirm that liblapack library is installed

# This module defines
# LIBLAPACK_FOUND, If false, don't try to use liblapack.

# Look for the header file installed by atlas package
# doesn't work for some reason
# CHECK_INCLUDE_FILE_CXX( clapack.h LIBLAPACK_FOUND )

#
# We seem to require both lapack _and_ blas, so only set LIBLAPACK_FOUND
# if both these two are found...
#

FIND_LIBRARY( LIBLAPACK_FOUND NAMES lapack PATHS /usr/lib /usr/local/lib )
FIND_LIBRARY( BLAS_FOUND      NAMES blas   PATHS /usr/lib /usr/local/lib )

IF( LIBLAPACK_FOUND )
  IF( BLAS_FOUND )
    MESSAGE( STATUS "Looking for liblapack - found")
  ELSE ( BLAS_FOUND )
    MESSAGE( STATUS "Looking for liblapack - found but blas is missing...")
    SET( LIBLAPACK_FOUND FALSE )
  ENDIF( BLAS_FOUND )
ELSE ( LIBLAPACK_FOUND )
  MESSAGE( STATUS "Looking for liblapack - not found")
ENDIF( LIBLAPACK_FOUND )
