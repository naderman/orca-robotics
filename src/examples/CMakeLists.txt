# A place for particularly-well documented and commented components
# which show how to use Orca libraries, etc.

MESSAGE ( STATUS "== EXAMPLES ==" )

# when adding new directories, please maintain alphabetical order
ADD_SUBDIRECTORY ( drunkrobot )
ADD_SUBDIRECTORY ( pureice )
# much easier to check for java here, because the example is build as a separate project
IF ( JAVA_FOUND AND BUILD_JAVA )
    ADD_SUBDIRECTORY ( pureicej )
ENDIF ( JAVA_FOUND AND BUILD_JAVA )
