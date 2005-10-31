OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" OFF )

IF( HAVE_OPENCV AND HAVE_CVAUX AND HAVE_HIGHGUI )
IF( BUILD_LASERFEATUREEXTRACTOR )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

# Not sure how to find "all .cpp files except main.cpp", have to do it by hand
SET( COMMON_SRCS extractorone featureextractorbase laserfeatureextractorcomponent polarfeature2d_i fakeextractor laserconsumer_i mainloop svd section polefinder )

# Files for the component
SET( COMP_SRCS main  COMMON_SRCS )

# stand-alone component  
GLOBAL_ADD_EXECUTABLE( laserfeatureextractor ${COMP_SRCS} )
TARGET_LINK_LIBRARIES( laserfeatureextractor ${HAVE_OPENCV} ${HAVE_CVAUX} ${HAVE_HIGHGUI} )

# Files for the service
SET( SERV_SRCS  laserfeatureextractorservice_i  COMMON_SRCS )

# build an IceBox service
ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED ${SERV_SRCS} )

INSTALL_TARGETS( /lib OrcaLaserFeatureExtractor )

ENDIF( BUILD_LASERFEATUREEXTRACTOR )
ENDIF( HAVE_OPENCV AND HAVE_CVAUX AND HAVE_HIGHGUI )
