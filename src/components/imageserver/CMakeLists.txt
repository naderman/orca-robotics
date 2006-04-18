OPTION( BUILD_IMAGE_SERVER "Try to build Image Server" OFF )

IF ( BUILD_IMAGE_SERVER )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  # Use libOrcaIce
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

  # camera driver checks
  IF ( 1394_FOUND )
    SET( 1394_LIBS raw1394 dc1394_control )
  ELSE ( 1394_FOUND )
    MESSAGE( "    libdc1394 not found: you will not be able to use firewire cameras" )
  ENDIF ( 1394_FOUND )

  IF ( NOT V4L_FOUND )
    MESSAGE( "    video4linux not found: you will not be able to use usb cameras" )
  ENDIF ( NOT V4L_FOUND )

  IF ( NOT 1394_FOUND AND NOT V4L_FOUND )
    MESSAGE( "     Video4Linux nor libdc1394 can be found: only the fake driver can be used" )
  ENDIF ( NOT 1394_FOUND AND NOT V4L_FOUND )

  # this is the only implementation so far
  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )

      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      IF( OPENCV7_FOUND )
        MESSAGE( "    Using new opencv (>= 0.9.7)" )
        MESSAGE( "    Make sure this version is from cvs and not the release" )

        # sublibrary for opencv grabber
        # only cvgrabber depends on opencv... might need to fix the
        # CMakeLists.txt in imagegrabber so that cvgrabber is not
        # built when opencv is not installed rather than the whole component
        SUBDIRS( imagegrabber )

        INCLUDE_DIRECTORIES( ${OPENCV7_INCLUDE_DIRS} )

        # LINK_LIBRARIES( ${OPENCV7_LIBS} imagegrabber )
        LINK_DIRECTORIES( imagegrabber )

        # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
        SET( COMMON_SRCS cameraI camerautil component conversions driver fakedriver mainloop monodriver )

        # Files for the stand-alone application
        SET( COMP_SRCS main ${COMMON_SRCS} )
        
        # Files for the icebox service
        SET( SERVICE_SRCS service ${COMMON_SRCS} )

        GLOBAL_ADD_EXECUTABLE( imageserver ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( imageserver ${OPENCV7_LIBS} imagegrabber )

        # build an IceBox service
        ADD_LIBRARY( OrcaImageServer SHARED ${SERVICE_SRCS} )

        # important to let linker know about this inter-library dependency
        # otherwise IceBox fails to load the service
        TARGET_LINK_LIBRARIES( OrcaImageServer ${OPENCV7_LIBS} imagegrabber )

        INSTALL_TARGETS( /lib OrcaImageServer )

        # test image for the fake driver to send
        INSTALL_PROGRAMS(/images penguin.jpg)

        GENERATE_FROM_DEF( imageserver.def )

      ELSE( OPENCV7_FOUND )
        MESSAGE( "ImageServer will not be built.")
        MESSAGE( "     OpenCv (0.9.7) has not been found. You need the cvs version." )
        SET ( BUILD_IMAGE_SERVER 0 )
      ENDIF( OPENCV7_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( BUILD_IMAGE_SERVER )
