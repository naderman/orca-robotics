# Confirm that liblapack library is installed

# This module defines
# LIBLAPACK_FOUND, If false, don't try to use liblapack.

# Look for the header file installed by atlas package
# doesn't work for some reason
# check_include_file_cxx( clapack.h LIBLAPACK_FOUND )

#
# We seem to require both lapack _and_ blas, so only set LIBLAPACK_FOUND
# if both these two are found...
#

FIND_LIBRARY( LIBLAPACK_FOUND NAMES lapack PATHS /usr/lib /usr/local/lib )
FIND_LIBRARY( BLAS_FOUND      NAMES blas   PATHS /usr/lib /usr/local/lib )

if( LIBLAPACK_FOUND )
  if( BLAS_FOUND )
    message( STATUS "Looking for liblapack - found")
  else( BLAS_FOUND )
    message( STATUS "Looking for liblapack - found but blas is missing...")
    set( LIBLAPACK_FOUND FALSE )
  endif( BLAS_FOUND )
else( LIBLAPACK_FOUND )
  message( STATUS "Looking for liblapack - not found")
endif( LIBLAPACK_FOUND )
