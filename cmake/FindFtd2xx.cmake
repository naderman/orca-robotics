# Confirm that libftd2xx library is installed

# This module defines
# FTD2XX_FOUND, If false, don't try to use liblapack.

# Look for boost
CHECK_INCLUDE_FILE_CXX( ftd2xx.h FTD2XX_FOUND )
IF ( FTD2XX_FOUND )
    MESSAGE( STATUS "Looking for libftd2xx - found")
ELSE ( FTD2XX_FOUND )
    MESSAGE( STATUS "Looking for libftd2xx - not found")
ENDIF ( FTD2XX_FOUND )
