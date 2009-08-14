# A place for well-tested and well-documented libraries.

message( STATUS "== DRIVERS ==" )

# when adding new directories, please maintain alphabetical order 
# unless there are inter-dependencies.

add_subdirectory( bicycleplayerclient )

add_subdirectory( disparitysimple )

add_subdirectory( gpsfake )
add_subdirectory( gpsgarmingbx )

add_subdirectory( humaninput2djoystick )
add_subdirectory( humaninput2dkbdiostream )
add_subdirectory( humaninput2dkbdtermio )

add_subdirectory( imagefake )
add_subdirectory( imagefile )
add_subdirectory( imagecvcamera )
add_subdirectory( imagedc1394camera )

add_subdirectory( multiimagecvcamera )
add_subdirectory( multiimagefake )
add_subdirectory( multiimagefile )

add_subdirectory( rangerarrayfake )
#add_subdirectory( rangerarrayplayersonarclient )

add_subdirectory( insgpsfake )
add_subdirectory( insgpsnovatelgbx )
add_subdirectory( insgpsnovatelspan )

add_subdirectory( laserscanner2dfake )
add_subdirectory( laserscanner2dhokuyoaist )
add_subdirectory( laserscanner2dplayerclient )
add_subdirectory( laserscanner2dsickcarmen )
add_subdirectory( laserscanner2dsickgbx )

add_subdirectory( laserfeatureextractorcombined )

add_subdirectory( mhipa10arcnet )
add_subdirectory( mhipa10fake )

add_subdirectory( pathplanner2dfake )
add_subdirectory( pathplanner2dacfr )

add_subdirectory( robot2dfake )
add_subdirectory( robot2dplayerclient )

add_subdirectory( segwayrmpfake )
add_subdirectory( segwayrmpacfr )
add_subdirectory( segwayrmpplayerclient )

