INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( logreplaytest logreplaytest.cpp )
TARGET_LINK_LIBRARIES( logreplaytest OrcaLog OrcaLogFactory )

# Need OpenCV to compress images to file
IF ( OPENCV_FOUND )
    INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
    TARGET_LINK_LIBRARIES( logreplaytest ${OPENCV_LIBS} OrcaImage )
    ADD_DEFINITIONS( -DOPENCV_FOUND )
ENDIF ( OPENCV_FOUND )

ADD_TEST( LogReplayTest logreplaytest )
GENERATE_CONFIG_FILE( logreplaytest.def logreplaytest 0 )
MESSAGE ( STATUS "Planning to Build Test     : LogReplayTest [libOrcaLogFactory]")

ADD_EXECUTABLE( defaultfactoryinstantiate defaultfactoryinstantiate.cpp )
TARGET_LINK_LIBRARIES( defaultfactoryinstantiate OrcaLog OrcaLogFactory )
ADD_TEST( DefaultFactoryInstantiate defaultfactoryinstantiate )
MESSAGE ( STATUS "Planning to Build Test     : DefaultFactoryInstantiate [libOrcaLogFactory]")
