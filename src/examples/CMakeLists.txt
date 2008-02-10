# A place for particularly-well documented and commented components
# which show how to use Orca libraries, etc.

GLOBAL_ADD_LICENSE( LGPL )

MESSAGE ( STATUS "== EXAMPLES ==" )

IF ( build_EXAMPLES )

    # when adding new directories, please maintain alphabetical order
    ADD_SUBDIRECTORY ( brick )
    ADD_SUBDIRECTORY ( orca2yarp )
    ADD_SUBDIRECTORY ( player2orca )
    ADD_SUBDIRECTORY ( pureice )
    # much easier to check for java here, because the example is build as a separate project
    IF ( JAVA_FOUND AND build_JAVA )
        ADD_SUBDIRECTORY ( pureicej )
    ENDIF ( JAVA_FOUND AND build_JAVA )
    # much easier to check for python here, because the example is build as a separate project
    IF ( PYTHON_FOUND AND build_PYTHON )
        ADD_SUBDIRECTORY ( pureicepy )
    ENDIF ( PYTHON_FOUND AND build_PYTHON )
    ADD_SUBDIRECTORY ( reactivewalker )

ELSE ( build_EXAMPLES )

    MESSAGE ( STATUS "Example compilation is disabled" )

ENDIF ( build_EXAMPLES )
