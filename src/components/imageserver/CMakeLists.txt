OPTION( BUILD_IMAGE_SERVER "Try to build Image Server" OFF )

IF ( BUILD_IMAGE_SERVER )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  # Use libOrcaIce
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

  # this is the only implementation so far
  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )

      # SUBDIRS( test ) 
      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      IF( OPENCV7_FOUND )
        MESSAGE( "    Using new opencv (>= 0.9.7)" )
        MESSAGE( "    Make sure this version is from cvs and not the release" )

        # sublibrary for opencv grabber
        SUBDIRS( imagegrabber )
        MESSAGE( "    Monocular camera driver for ImageServer - can be built" )

        INCLUDE_DIRECTORIES( ${OPENCV7_INCLUDE_DIRS} )

        # need a check here for monodriver being built
        #        LINK_LIBRARIES( ${OPENCV7_LIBS} camera ${1394_LIBS} imagegrabber )
        IF ( 1394_FOUND )
          SET( 1394_LIBS raw1394 dc1394_control )
          LINK_LIBRARIES( ${OPENCV7_LIBS} ${1394_LIBS} imagegrabber )
        ELSE ( 1394_FOUND )
          MESSAGE( "    libdc1394 not found: you will not be able to use firewire cameras" )
          LINK_LIBRARIES( ${OPENCV7_LIBS} ${1394_LIBS} imagegrabber )
        ENDIF ( 1394_FOUND )

         LINK_DIRECTORIES( imagegrabber )

        #
        # This gathers all the source files into the variable 'SRCS'
        #
        FILE( GLOB SRCS *.cpp )

        GLOBAL_ADD_EXECUTABLE( imageserver ${SRCS} )
        GENERATE_FROM_DEF( imageserver.def )

        # test image for the fake driver to send
        INSTALL_PROGRAMS(/images penguin.jpg)

      ELSE( OPENCV7_FOUND )
        MESSAGE( "ImageServer will not be built.")
        MESSAGE( "     OpenCv (0.9.7) has not been found. You need the cvs version." )
        SET ( BUILD_IMAGE_SERVER 0)
      ENDIF( OPENCV7_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( BUILD_IMAGE_SERVER )
