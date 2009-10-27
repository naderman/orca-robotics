# A place for particularly-well documented and commented components
# which show how to use Orca libraries, etc.

GBX_ADD_LICENSE( LGPL )

message( STATUS "== EXAMPLES ==" )

if( ORCA_BUILD_EXAMPLES )

    # when adding new directories, please maintain alphabetical order
    add_subdirectory( brick )
    add_subdirectory( orca2yarp )
    add_subdirectory( player2orca )
    add_subdirectory( pureice )
    # much easier to check for java here, because the example is build as a separate project
    if( JAVA_FOUND AND ORCA_BUILD_JAVA )
        add_subdirectory( pureicej )
    endif( JAVA_FOUND AND ORCA_BUILD_JAVA )
    # much easier to check for python here, because the example is build as a separate project
    if( PYTHON_FOUND AND build_PYTHON )
        add_subdirectory( pureicepy )
    endif( PYTHON_FOUND AND build_PYTHON )
    add_subdirectory( reactivewalker )

else( ORCA_BUILD_EXAMPLES )

    message( STATUS "Example compilation is disabled" )

endif( ORCA_BUILD_EXAMPLES )
