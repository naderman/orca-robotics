OPTION( BUILD_IMAGE_SERVER "Try to build Image Server" ON )

IF ( BUILD_IMAGE_SERVER )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  # Use libOrcaIce
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

  #
  # checks for libraries and drivers
  #

  # low level camera driver checks
  IF ( 1394_FOUND )
    SET( 1394_LIBS raw1394 dc1394_control )
  ELSE ( 1394_FOUND )
    MESSAGE( "    libdc1394 not found: you will not be able to use firewire cameras" )
  ENDIF ( 1394_FOUND )

  IF ( NOT V4L_FOUND )
    MESSAGE( "    video4linux not found: you will not be able to use usb cameras" )
  ENDIF ( NOT V4L_FOUND )

  # opencv image grabber checks
  IF ( OPENCV7_FOUND ) 
    MESSAGE( "    OpenCv-0.9.7 Image Grabber - can be built" )
  ELSE ( OPENCV7_FOUND ) 
    MESSAGE( "    OpenCv-0.9.7 Image Grabber - cannot be built" )
  ENDIF ( OPENCV7_FOUND ) 

  # digiclops image grabber library checks
  IF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
    MESSAGE( "    The Digiclops Image Grabber - can be built" )
  ELSE ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
    MESSAGE( "    The Digiclops Image Grabber - cannot be built" )
  ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 

  # automatically generate #ifdef's for component.cpp
  CONFIGURE_FILE( configimageserver.h.cmake ${PROJ_SOURCE_DIR}/src/components/imageserver/configimageserver.h )

  #
  # orca drivers and image grabbers to be built
  # 

  # specify the orca image grabbers to be built
  IF ( V4L_FOUND OR 1394_FOUND )
    IF ( OPENCV7_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
      
      SET ( DRIVER_SRCS driver fakedriver monodriver)

      SUBDIRS( imagegrabber )

    ELSE ( OPENCV7_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
      MESSAGE ( "     Image grabbing libraries cannot be found... only the fake driver can be built." ) 
      MESSAGE ( "     Please install the cvs version of opencv for monocular cameras or the digiclops libraries for the trinocular digiclops camera."  ) 
      SET ( DRIVER_SRCS fakedriver )
    ENDIF ( OPENCV7_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )

  ELSE ( V4L_FOUND OR 1394_FOUND )
    MESSAGE ( "     Image grabbing drivers cannot be found... only the fake driver can be built." ) 
    MESSAGE ( "     Please install Video4Linux for usb cameras and/or libdc1394 for firewire cameras"  ) 
    SET ( DRIVER_SRCS fakedriver )

  ENDIF ( V4L_FOUND OR 1394_FOUND )

  # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
  SET( COMMON_SRCS cameraI camerautil component conversions mainloop ${DRIVER_SRCS} )

  # Files for the stand-alone application
  SET( COMP_SRCS main ${COMMON_SRCS} )
  
  # Files for the icebox service
  SET( SERVICE_SRCS service ${COMMON_SRCS} )

  GLOBAL_ADD_EXECUTABLE( imageserver ${COMP_SRCS} )

  # build an IceBox service
  ADD_LIBRARY( OrcaImageServer SHARED ${SERVICE_SRCS} )

  #
  # includes and linking for the various grabbing libraries
  #

  IF ( OPENCV7_FOUND )
    INCLUDE_DIRECTORIES( ${OPENCV7_INCLUDE_DIRS} )
    TARGET_LINK_LIBRARIES( imageserver ${OPENCV7_LIBS} imagegrabber )
    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaImageServer ${OPENCV7_LIBS} imagegrabber )
  ENDIF ( OPENCV7_FOUND )

  IF ( DIGICLOPS_AND_TRICLOPS_FOUND )
    INCLUDE_DIRECTORIES( /usr/local/include/digiclops /usr/local/include/triclops)
    TARGET_LINK_LIBRARIES( imageserver digiclops triclops imagegrabber )
    TARGET_LINK_LIBRARIES( OrcaImageServer digiclops triclops imagegrabber )
  ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND )

  IF ( OPENCV7_FOUND AND DIGICLOPS_AND_TRICLOPS_FOUND )
    INCLUDE_DIRECTORIES( ${OPENCV7_INCLUDE_DIRS} /usr/local/include/digiclops /usr/local/include/triclops)
    TARGET_LINK_LIBRARIES( imageserver ${OPENCV7_LIBS} digiclops triclops imagegrabber )
    TARGET_LINK_LIBRARIES( OrcaImageServer ${OPENCV7_LIBS} digiclops triclops imagegrabber )
  ENDIF ( OPENCV7_FOUND AND DIGICLOPS_AND_TRICLOPS_FOUND )

  #
  # misc
  #

  INSTALL_TARGETS( /lib OrcaImageServer )
  
  # test image for the fake driver to send
  INSTALL_PROGRAMS(/images penguin.jpg)

  GENERATE_FROM_DEF( imageserver.def )

ENDIF ( BUILD_IMAGE_SERVER )
