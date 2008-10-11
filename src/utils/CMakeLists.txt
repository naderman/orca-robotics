# A place for well-tested and well-documented utility applications.

MESSAGE( STATUS "== UTILITIES ==" )

# when adding new directories, please maintain alphabetical order 
# unless there are inter-dependencies.

ADD_SUBDIRECTORY( cleanregistry )
ADD_SUBDIRECTORY( deftools )
ADD_SUBDIRECTORY( iceping )
ADD_SUBDIRECTORY( logplayer )
ADD_SUBDIRECTORY( logrecorder )
ADD_SUBDIRECTORY( orcaview2d )
ADD_SUBDIRECTORY( orcaview3d )
ADD_SUBDIRECTORY( pinger )
ADD_SUBDIRECTORY( probe )
ADD_SUBDIRECTORY( registrylist )
ADD_SUBDIRECTORY( registryview )
ADD_SUBDIRECTORY( replier )
ADD_SUBDIRECTORY( slice2init )
ADD_SUBDIRECTORY( slice2string )
ADD_SUBDIRECTORY( snapshotlogger )
ADD_SUBDIRECTORY( systemtracermon )

# putting it after all subdir's because of cmake weirdness:
# when placed above, only one subdir license appears 
GBX_ADD_LICENSE( LGPL )
