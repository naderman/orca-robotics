#
# Links to libIce only.
# see also: UseIceBasics.cmake
#

INCLUDE( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

IF ( NOT ORCA_OS_WIN )

    LINK_LIBRARIES( Ice IceUtil )

ELSE ( NOT ORCA_OS_WIN )

    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized Ice       debug Iced
                    optimized IceUtil   debug IceUtild )

ENDIF ( NOT ORCA_OS_WIN )