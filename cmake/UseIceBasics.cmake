#
# Links to Ice basics defined as "what libOrcaIce uses"
# (and therefore all standard components use)
#

INCLUDE( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

IF ( NOT OS_WIN )

    LINK_LIBRARIES( Ice IceUtil IceStorm IceBox IceGrid )

ELSE ( NOT OS_WIN )

    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized Ice       debug Iced
                    optimized IceUtil   debug IceUtild 
                    optimized IceStorm  debug IceStormd 
                    optimized IceBox    debug IceBoxd
                    optimized IceGrid   debug IceGridd )

ENDIF ( NOT OS_WIN )