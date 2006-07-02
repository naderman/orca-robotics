# A place for components and utilities which are experimental, untested,
# undocumented, or all of the above. The option BUILD_SANDBOX is OFF
# by default.

MESSAGE ( STATUS "== SANDBOX ==" )

IF ( NOT WIN32 AND BUILD_SANDBOX )

    # when adding new directories, please maintain alphabetical order
    SUBDIRS(
	gorca
	netcam
	qorcauml
	reactivewalker
    )
    
ELSE ( NOT WIN32 AND BUILD_SANDBOX )

    MESSAGE ( STATUS "Sandbox compilation is disabled" )

ENDIF ( NOT WIN32 AND BUILD_SANDBOX )
