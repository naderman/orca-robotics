# Confirm that libftd2xx library is installed
#
# This module defines
# DC1394_FOUND, If false, don't try to use dc1394

include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

find_path( DC1394_PATH dc1394.h
# installation selected by user
$ENV{DC1394_HOME}/include
# system placed in /usr/include
/usr/include/dc1394
# system placed in /usr/local/include
/usr/local/include/dc1394
)

if( DC1394_PATH )
    message( STATUS "Looking for libdc1394-2 - found")
    SET ( DC1394_FOUND 1 )
else( DC1394_PATH )
    message( STATUS "Looking for libdc1394-2 - not found")
    SET ( DC1394_FOUND 0 )
endif( DC1394_PATH )