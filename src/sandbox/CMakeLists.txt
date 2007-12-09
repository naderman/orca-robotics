# A place for components and utilities which are experimental, untested,
# undocumented, unmaintaned, or all of the above. 
# The option BUILD_SANDBOX is OFF by default.

MESSAGE ( STATUS "== SANDBOX ==" )

IF ( NOT OS_WIN AND BUILD_SANDBOX )

    # when adding new directories, please maintain alphabetical order
    ADD_SUBDIRECTORY ( gorca )
    ADD_SUBDIRECTORY ( noisyrobot2d )
    ADD_SUBDIRECTORY ( pathplantester )
    ADD_SUBDIRECTORY ( pointcloudserver )
    ADD_SUBDIRECTORY ( pointcloudviewer ) 

ELSE ( NOT OS_WIN AND BUILD_SANDBOX )

    MESSAGE ( STATUS "Sandbox compilation is disabled" )

ENDIF ( NOT OS_WIN AND BUILD_SANDBOX )
