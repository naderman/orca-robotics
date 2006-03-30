OPTION( BUILD_IMAGE_VIEWER "Try to build Image Viewer" OFF )

IF ( BUILD_IMAGE_VIEWER )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )
    IF ( OPENCV_FOUND AND HIGHGUI_FOUND )
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
      LINK_LIBRARIES( ${OPENCV_LIBS} )


      #
      # This gathers all the source files into the variable 'SRCS'
      #
      FILE( GLOB SRCS *.cpp )

      GLOBAL_ADD_EXECUTABLE( imageviewer ${SRCS} )
      # GLOBAL_ADD_EXECUTABLE( imageviewer main cvdisplayer )
      GENERATE_FROM_DEF( imageviewer.def )

    ENDIF ( OPENCV_FOUND AND HIGHGUI_FOUND )

  ENDIF( OPENCV_IMPLEMENTATION )

  #
  # This gathers all the source files into the variable 'SRCS'
  #
  # FILE( GLOB SRCS *.cpp )

  # GLOBAL_ADD_EXECUTABLE( imageviewer ${SRCS} )


ENDIF ( BUILD_IMAGE_VIEWER )
