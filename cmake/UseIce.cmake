#
# Links to libIce only.
# see also: UseIceBasics.cmake
#

include( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

if( NOT ORCA_OS_WIN )

    link_libraries( Ice IceUtil )

else( NOT ORCA_OS_WIN )

    # windows... have to link to different libs depending on build type
    link_libraries( optimized Ice       debug Iced
                    optimized IceUtil   debug IceUtild )

endif( NOT ORCA_OS_WIN )