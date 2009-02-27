INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaLog OrcaLogFactory )

#ADD_EXECUTABLE( logreplaytest logreplaytest.cpp )
## Need OpenCV to compress images to file
# INCLUDE( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
#IF( OPENCV_FOUND )
#    INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
#    TARGET_LINK_LIBRARIES( logreplaytest ${OPENCV_LIBS} OrcaImage )
#    ADD_DEFINITIONS( -DOPENCV_FOUND )
#ENDIF( OPENCV_FOUND )
#GBX_ADD_TEST( LogReplayTest logreplaytest )
#ORCA_GENERATE_CONFIG_FILE( logreplaytest.def logreplaytest 0 )

ADD_EXECUTABLE( defaultautofactoryinstantiate defaultautofactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaLogFactory_AutoFactoryInstantiate defaultautofactoryinstantiate )

ADD_EXECUTABLE( defaultsnapshotfactoryinstantiate defaultsnapshotfactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaLogFactory_SnapshotFactoryInstantiate defaultsnapshotfactoryinstantiate )
