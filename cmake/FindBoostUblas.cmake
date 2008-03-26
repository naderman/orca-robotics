# Confirm that boost library is installed

# This module defines
# BOOST_UBLAS_FOUND, If false, don't try to use liblapack.

CHECK_INCLUDE_FILE_CXX( boost/numeric/ublas/matrix.hpp BOOST_UBLAS_FOUND )

IF( BOOST_UBLAS_FOUND )
    MESSAGE( STATUS "Looking for Boost ublas - found")
ELSE ( BOOST_UBLAS_FOUND )
    MESSAGE( STATUS "Looking for Boost ublas - not found")
ENDIF( BOOST_UBLAS_FOUND )
