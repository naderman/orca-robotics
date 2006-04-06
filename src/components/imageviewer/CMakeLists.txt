OPTION( BUILD_IMAGE_VIEWER "Try to build Image Viewer" OFF )

IF ( BUILD_IMAGE_VIEWER )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

  # this is the only implemenation so far
  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )
      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      IF( OPENCV7_FOUND )
        MESSAGE( "    Using new opencv (>= 0.9.7)" )
        MESSAGE( "    Make sure this version is from cvs and not the release" )
        INCLUDE_DIRECTORIES( ${OPENCV7_INCLUDE_DIRS} )

        LINK_LIBRARIES( ${OPENCV7_LIBS} )

        #
        # This gathers all the source files into the variable 'SRCS'
        #
        FILE( GLOB SRCS *.cpp )

        GLOBAL_ADD_EXECUTABLE( imageviewer ${SRCS} )
        GENERATE_FROM_DEF( imageviewer.def )

      ELSE( OPENCV7_FOUND )
        MESSAGE( "ImageViewer will not be built.")
        MESSAGE( "     OpenCv (0.9.7) has not been found. You need the cvs version." )
        SET ( BUILD_IMAGE_VIEWER 0)
      ENDIF( OPENCV7_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( BUILD_IMAGE_VIEWER )
