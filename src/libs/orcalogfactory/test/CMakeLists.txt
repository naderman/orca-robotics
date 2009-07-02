include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

link_libraries( OrcaLog OrcaLogFactory )

# This test used to do end-to-end test of selected log writers/readers.
# It's not longer maintained.
#add_executable( logreplaytest logreplaytest.cpp )
## Need OpenCV to compress images to file
# include( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
#if( OPENCV_FOUND )
#    include_directories( ${OPENCV_INCLUDE_DIRS} )
#    target_link_libraries( logreplaytest ${OPENCV_LIBS} OrcaImage )
#    add_definitions( -DOPENCV_FOUND )
#endif( OPENCV_FOUND )
#GBX_ADD_TEST( LogReplayTest logreplaytest )
#ORCA_GENERATE_CONFIG_FILE( logreplaytest.def logreplaytest 0 )

add_executable( defaultautofactoryinstantiate defaultautofactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaLogFactory_AutoFactoryInstantiate defaultautofactoryinstantiate )

add_executable( defaultsnapshotfactoryinstantiate defaultsnapshotfactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaLogFactory_SnapshotFactoryInstantiate defaultsnapshotfactoryinstantiate )
