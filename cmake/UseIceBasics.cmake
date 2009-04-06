#
# Links to Ice basics defined as "what libOrcaIce uses"
# (and therefore all standard components use)
#

include( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )
include( ${ORCA_CMAKE_DIR}/UseIce.cmake )

if( NOT ORCA_OS_WIN )

    link_libraries( IceStorm IceBox IceGrid Glacier2 )

else( NOT ORCA_OS_WIN )

    # windows... have to link to different libs depending on build type
    link_libraries( optimized IceStorm  debug IceStormd 
                    optimized IceBox    debug IceBoxd
                    optimized IceGrid   debug IceGridd
                    optimized Glacier2   debug Glacier2d )

endif( NOT ORCA_OS_WIN )