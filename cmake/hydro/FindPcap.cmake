# Confirm that <pcap.h> is installed
#
# This module defines
# PCAP_FOUND, If false, don't try to use pcap.so

include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

# Look for the library, set the Variable if it exists
check_include_file_cxx( pcap.h PCAP_FOUND )
if( PCAP_FOUND )
    message( STATUS "Looking for pcap.h - found")
else( PCAP_FOUND )
    message( STATUS "Looking for pcap.h - not found")
endif( PCAP_FOUND )
