# A place for components and utilities which are experimental, untested,
# undocumented, unmaintaned, or all of the above. 
# The option ORCA_BUILD_SANDBOX is OFF by default.

MESSAGE( STATUS "== SANDBOX ==" )

IF( NOT GBX_OS_WIN AND ORCA_BUILD_SANDBOX )

    # when adding new directories, please maintain alphabetical order
    ADD_SUBDIRECTORY( slice2string )

ELSE ( NOT GBX_OS_WIN AND ORCA_BUILD_SANDBOX )

    MESSAGE( STATUS "Sandbox compilation is disabled" )

ENDIF( NOT GBX_OS_WIN AND ORCA_BUILD_SANDBOX )


# putting it after all subdir's because of cmake weirdness:
# when placed above, only one subdir license appears 
GBX_ADD_LICENSE( LGPL )
