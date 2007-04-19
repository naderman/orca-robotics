# A place for components and utilities which are experimental, untested,
# undocumented, unmaintaned, or all of the above. The option BUILD_SANDBOX is OFF
# by default.

MESSAGE ( STATUS "== SANDBOX ==" )

IF ( NOT OS_WIN AND BUILD_SANDBOX )

    # when adding new directories, please maintain alphabetical order
    ADD_SUBDIRECTORY ( drunkrobot )
    ADD_SUBDIRECTORY ( featuremap2dmon )
    ADD_SUBDIRECTORY ( gorca )
    ADD_SUBDIRECTORY ( netcam )
    ADD_SUBDIRECTORY ( reactivewalker )
    # ADD_SUBDIRECTORY ( noisyrobot2d )
    ADD_SUBDIRECTORY ( pointcloudserver )
    ADD_SUBDIRECTORY ( pointcloudviewer ) 
    # ADD_SUBDIRECTORY ( robot2dtester )
    
ELSE ( NOT OS_WIN AND BUILD_SANDBOX )

    MESSAGE ( STATUS "Sandbox compilation is disabled" )

ENDIF ( NOT OS_WIN AND BUILD_SANDBOX )
