OPTION( BUILD_IMAGE_SERVER "Try to build image server" ON )

IF ( BUILD_IMAGE_SERVER )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
# Use libOrcaIce
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

#IF ( LIBCAMERA_FOUND )
  # Has no outside dependencies, so we can always build it
  # SUBDIRS( monofirewire )
  # LINK_DIRECTORIES( monofirewire )

  IF ( 1394_FOUND )
    SET( 1394_LIBS raw1394 dc1394_control )
  ENDIF ( 1394_FOUND )

  # need to have a check here for low level video drivers. For linux
  # need 1394_FOUND=true or v4l drivers found. Need to add the v4l
  # check in the higher level cmake checks.
  # IF ( 1394_FOUND OR V4L_FOUND )
  SUBDIRS(  imagegrabber )
  MESSAGE( "    Monocular camera driver for ImageServer - can be built" )
  # ENDIF ( 1394_FOUND OR V4L_FOUND )

  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )
    IF ( OPENCV_FOUND AND HIGHGUI_FOUND )

      # SUBDIRS( test ) 
      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      PKGCONFIG("opencv" OPENCV_FOUND OPENCV_INCLUDE_DIRS OPENCV_DEFINES OPENCV_LINK_DIRS OPENCV_LIBS)
      IF( OPENCV_FOUND )
        MESSAGE( "Using new opencv (>= 0.9.6)" )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        SET( NEW_OPENCV 1 )
      ELSE( OPENCV_FOUND )
        MESSAGE( "Using old opencv (0.9.5)" )
        SET( OLD_OPENCV 1 )
        SET( OPENCV_LIBS opencv highgui )
        # INCLUDE_DIRECTORIES( /usr/include/opencv /usr/local/include/opencv )
        ADD_DEFINITIONS( -DOLD_OPENCV )
        SET( OPENCV_FOUND 1)
      ENDIF( OPENCV_FOUND )  

      #       INCLUDE_DIRECTORIES( ${PROJ_SOURCE_DIR}/src/util/imageutil )
      #       LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/util/imageutil )
      #       LINK_LIBRARIES( ${OPENCV_LIBS} imageutil camera )
      # need a check here for monodriver being built
      LINK_LIBRARIES( ${OPENCV_LIBS} camera ${1394_LIBS} imagegrabber )
      LINK_DIRECTORIES( imagegrabber )

      #
      # This gathers all the source files into the variable 'SRCS'
      #
      FILE( GLOB SRCS *.cpp )

      GLOBAL_ADD_EXECUTABLE( imageserver ${SRCS} )
      GENERATE_FROM_DEF( imageserver.def )

      INSTALL_PROGRAMS(/images penguin.jpg)

    ENDIF ( OPENCV_FOUND AND HIGHGUI_FOUND )

  ENDIF( OPENCV_IMPLEMENTATION )

  #ELSE ( LIBCAMERA_FOUND )
  #  MESSAGE( "Not Building ImageServer -- LibCamera not found." )
  #ENDIF ( LIBCAMERA_FOUND )


ENDIF ( BUILD_IMAGE_SERVER )
