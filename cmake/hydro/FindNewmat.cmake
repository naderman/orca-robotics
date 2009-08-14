# Locate Newmat install directory

# This module defines
# NEWMAT_HOME where to find include, lib, bin, etc.
# NEWMAT_FOUND, If false, don't try to use Ice.

INCLUDE (${HYDRO_CMAKE_DIR}/FindPkgConfig.cmake)

if( CMAKE_PKGCONFIG_EXECUTABLE )

    # Find all the newmat stuff with pkg-config
    PKGCONFIG( "newmat >= 11.0.0" NEWMAT_FOUND NEWMAT_INCLUDE_DIRS NEWMAT_DEFINES NEWMAT_LINK_DIRS NEWMAT_LIBS )

    if( NEWMAT_FOUND )
       message( STATUS "   Includes in: ${NEWMAT_INCLUDE_DIRS}")
       message( STATUS "   Libraries in: ${NEWMAT_LINK_DIRS}")
       message( STATUS "   Libraries: ${NEWMAT_LIBS}")
       message( STATUS "   Defines: ${NEWMAT_DEFINES}")
    endif( NEWMAT_FOUND )

else( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually

endif( CMAKE_PKGCONFIG_EXECUTABLE )
