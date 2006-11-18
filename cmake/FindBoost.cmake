# Confirm that boost library is installed

# This module defines
# BOOST_FOUND, If false, don't try to use liblapack.

CHECK_INCLUDE_FILE_CXX( boost/numeric/ublas/matrix.hpp BOOST_FOUND )

IF ( BOOST_FOUND )
    MESSAGE( STATUS "Looking for Boost - found")
ELSE ( BOOST_FOUND )
    MESSAGE( STATUS "Looking for Boost - not found")
ENDIF ( BOOST_FOUND )
