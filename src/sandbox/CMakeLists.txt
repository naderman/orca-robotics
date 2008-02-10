# A place for components and utilities which are experimental, untested,
# undocumented, unmaintaned, or all of the above. 
# The option build_SANDBOX is OFF by default.

MESSAGE ( STATUS "== SANDBOX ==" )

IF ( NOT ORCA_OS_WIN AND build_SANDBOX )

    # when adding new directories, please maintain alphabetical order
    ADD_SUBDIRECTORY ( insgps )

ELSE ( NOT ORCA_OS_WIN AND build_SANDBOX )

    MESSAGE ( STATUS "Sandbox compilation is disabled" )

ENDIF ( NOT ORCA_OS_WIN AND build_SANDBOX )


# putting it after all subdir's because of cmake weirdness:
# when placed above, only one subdir license appears 
GLOBAL_ADD_LICENSE( LGPL )
