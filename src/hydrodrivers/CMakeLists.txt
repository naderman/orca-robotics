# A place for well-tested and well-documented libraries.

message( STATUS "== HYDRO DRIVERS ==" )

# when adding new directories, please maintain alphabetical order
# unless there are inter-dependencies.

add_subdirectory( hydrobicycleplayerclient )

add_subdirectory( hydrodisparitysimple )

add_subdirectory( hydrogpsfake )
add_subdirectory( hydrogpsgarmingbx )

add_subdirectory( hydrohumaninput2djoystick )
add_subdirectory( hydrohumaninput2dkbdiostream )
add_subdirectory( hydrohumaninput2dkbdtermio )

add_subdirectory( hydroimagefake )
add_subdirectory( hydroimagefile )
add_subdirectory( hydroimagecvcamera )
add_subdirectory( hydroimagedc1394camera )

add_subdirectory( hydromultiimagecvcamera )
add_subdirectory( hydromultiimagedc1394camera )
add_subdirectory( hydromultiimagefake )
add_subdirectory( hydromultiimagefile )

add_subdirectory( hydrorangerarrayfake )
#add_subdirectory( hydrorangerarrayplayersonarclient )

add_subdirectory( hydroinsgpsfake )
add_subdirectory( hydroinsgpsnovatelgbx )

add_subdirectory( hydrolaserscanner2dfake )
add_subdirectory( hydrolaserscanner2dhokuyoaist )
add_subdirectory( hydrolaserscanner2dplayerclient )
add_subdirectory( hydrolaserscanner2dsickcarmen )
add_subdirectory( hydrolaserscanner2dsickgbx )

add_subdirectory( hydrolaserfeatureextractorcombined )

add_subdirectory( hydromhipa10arcnet )
add_subdirectory( hydromhipa10fake )

add_subdirectory( hydropathplanner2dfake )
add_subdirectory( hydropathplanner2dacfr )

add_subdirectory( hydrorobot2dfake )
add_subdirectory( hydrorobot2dplayerclient )

add_subdirectory( hydrosegwayrmpfake )
add_subdirectory( hydrosegwayrmpacfr )
add_subdirectory( hydrosegwayrmpplayerclient )

