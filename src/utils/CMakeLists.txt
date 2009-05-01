# A place for well-tested and well-documented utility applications.

message( STATUS "== UTILITIES ==" )

# when adding new directories, please maintain alphabetical order 
# unless there are inter-dependencies.

add_subdirectory( deftools )
add_subdirectory( def2uml )
add_subdirectory( iceping )
add_subdirectory( logplayer )
add_subdirectory( logrecorder )
add_subdirectory( orcaview2d )
add_subdirectory( orcaview3d )
add_subdirectory( pinger )
add_subdirectory( probe )
add_subdirectory( registryclean )
add_subdirectory( registrylist )
add_subdirectory( registryview )
add_subdirectory( replier )
add_subdirectory( slice2orca )
add_subdirectory( snapshotlogger )

# putting it after all subdir's because of cmake weirdness:
# when placed above, only one subdir license appears 
GBX_ADD_LICENSE( LGPL )
