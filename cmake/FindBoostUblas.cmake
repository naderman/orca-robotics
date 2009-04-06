# Confirm that boost library is installed

# This module defines
# BOOST_UBLAS_FOUND, If false, don't try to use liblapack.

check_include_file_cxx( boost/numeric/ublas/matrix.hpp BOOST_UBLAS_FOUND )

if( BOOST_UBLAS_FOUND )
    message( STATUS "Looking for Boost ublas - found")
else( BOOST_UBLAS_FOUND )
    message( STATUS "Looking for Boost ublas - not found")
endif( BOOST_UBLAS_FOUND )
