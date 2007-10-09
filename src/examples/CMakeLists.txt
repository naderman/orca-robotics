# A place for particularly-well documented and commented components
# which show how to use Orca libraries, etc.

MESSAGE ( STATUS "== EXAMPLES ==" )

IF ( BUILD_EXAMPLES )

    # when adding new directories, please maintain alphabetical order
    ADD_SUBDIRECTORY ( brick )
    ADD_SUBDIRECTORY ( orca2yarp )
    ADD_SUBDIRECTORY ( player2orca )
    ADD_SUBDIRECTORY ( pureice )
    # much easier to check for java here, because the example is build as a separate project
    IF ( JAVA_FOUND AND BUILD_JAVA )
        ADD_SUBDIRECTORY ( pureicej )
    ENDIF ( JAVA_FOUND AND BUILD_JAVA )
    # much easier to check for python here, because the example is build as a separate project
    IF ( PYTHON_FOUND AND BUILD_PYTHON )
        ADD_SUBDIRECTORY ( pureicepython )
    ENDIF ( PYTHON_FOUND AND BUILD_PYTHON )
    ADD_SUBDIRECTORY ( reactivewalker )

ELSE ( BUILD_EXAMPLES )

    MESSAGE ( STATUS "Example compilation is disabled" )

ENDIF ( BUILD_EXAMPLES )
