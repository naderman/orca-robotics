#Build the stand alone test app if needed!
# (Always build so it's lying around just in case)
#if( ORCA_BUILD_TESTS )
    add_subdirectory( test )
#endif( ORCA_BUILD_TESTS )
