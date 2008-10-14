# A place for well-tested and well-documented components.

MESSAGE( STATUS "== COMPONENTS ==" )

# when adding new directories, please maintain alphabetical order
ADD_SUBDIRECTORY( cameraserver )
ADD_SUBDIRECTORY( faithlocaliser )
ADD_SUBDIRECTORY( featuremaploader )
ADD_SUBDIRECTORY( goalplanner )
ADD_SUBDIRECTORY( gps )
ADD_SUBDIRECTORY( gps2localise2d )
ADD_SUBDIRECTORY( imageserver )
#ADD_SUBDIRECTORY( imageviewer )
ADD_SUBDIRECTORY( insgps )
ADD_SUBDIRECTORY( laser2d )
ADD_SUBDIRECTORY( laser2og )
ADD_SUBDIRECTORY( laserfeatureextractor )
ADD_SUBDIRECTORY( lasermon )
ADD_SUBDIRECTORY( localnav )
ADD_SUBDIRECTORY( ogmaploader )
ADD_SUBDIRECTORY( ognode )
ADD_SUBDIRECTORY( orcamon )
ADD_SUBDIRECTORY( pathplanner )
ADD_SUBDIRECTORY( pingserver )
ADD_SUBDIRECTORY( pixmaploader )
ADD_SUBDIRECTORY( propertyserver )
ADD_SUBDIRECTORY( robot2d )
ADD_SUBDIRECTORY( segwayrmp )
ADD_SUBDIRECTORY( simlocaliser )
ADD_SUBDIRECTORY( teleop )
ADD_SUBDIRECTORY( tracermon )
ADD_SUBDIRECTORY( wifi )

# putting it after all subdir's because of cmake weirdness:
# when placed above, only one subdir license appears 
GBX_ADD_LICENSE( LGPL )
