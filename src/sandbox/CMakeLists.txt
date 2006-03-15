# A place for components and utilities which are experimental, untested,
# undocumented, or all of the above. The option BUILD_SANDBOX is OFF
# by default.

MESSAGE ( "== SANDBOX ==" )

IF ( BUILD_SANDBOX )

    # when adding new directories, please maintain alphabetical order
    SUBDIRS(
    missioncontroller
	netcam
	reactivewalker
	pathplanner
	stagelocaliser
    )
    
ELSE ( BUILD_SANDBOX )

    MESSAGE ( "Sandbox compilation is disabled" )

ENDIF ( BUILD_SANDBOX )
