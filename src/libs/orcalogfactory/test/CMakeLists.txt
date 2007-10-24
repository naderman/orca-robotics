INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

LINK_LIBRARIES( OrcaLog OrcaLogFactory )

#ADD_EXECUTABLE( logreplaytest logreplaytest.cpp )
## Need OpenCV to compress images to file
#IF ( OPENCV_FOUND )
#    INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
#    TARGET_LINK_LIBRARIES( logreplaytest ${OPENCV_LIBS} OrcaImage )
#    ADD_DEFINITIONS( -DOPENCV_FOUND )
#ENDIF ( OPENCV_FOUND )
#GLOBAL_ADD_TEST( LogReplayTest logreplaytest )
#GENERATE_CONFIG_FILE( logreplaytest.def logreplaytest 0 )

ADD_EXECUTABLE( defaultfactoryinstantiate defaultfactoryinstantiate.cpp )
GLOBAL_ADD_TEST( DefaultFactoryInstantiate defaultfactoryinstantiate )
