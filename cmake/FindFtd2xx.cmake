# Confirm that libftd2xx library is installed

# This module defines
# FTD2XX_FOUND, If false, don't try to use liblapack.

check_include_file_cxx( ftd2xx.h FTD2XX_FOUND )
if( FTD2XX_FOUND )
    message( STATUS "Looking for libftd2xx - found")
else( FTD2XX_FOUND )
    message( STATUS "Looking for libftd2xx - not found")
endif( FTD2XX_FOUND )
