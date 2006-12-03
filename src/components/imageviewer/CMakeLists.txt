OPTION( BUILD_IMAGE_VIEWER "Try to build Image Viewer" ON )

IF ( NOT WIN32 AND BUILD_IMAGE_VIEWER )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

  # this is the only implemenation so far
  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )
      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      IF( OPENCV_FOUND )
        MESSAGE( STATUS "    Using new opencv (>= 0.9.9)" )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )

        IF ( TRICLOPS_FOUND )
          MESSAGE( STATUS "    The Triclops libraries have been found - you will be able to view images from a digiclops camera")
        ELSE ( TRICLOPS_FOUND )
          MESSAGE( STATUS "    The Triclops libraries have not been found - you will not be able to view images from a digiclops camera")
        ENDIF ( TRICLOPS_FOUND )

        INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

        # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
        # SET( COMMON_SRCS cameraconsumerI component imagehandler colourconversions conversions  imageutils)
        SET( COMMON_SRCS cameraconsumerI component mainloop )

        GLOBAL_ADD_EXECUTABLE( imageviewer main ${COMMON_SRCS} )

        # build an IceBox service
        GLOBAL_ADD_LIBRARY( OrcaImageViewer SHARED service ${COMMON_SRCS} )

        # link libraries
        TARGET_LINK_LIBRARIES( imageviewer ${OPENCV_LIBS} OrcaImage)
        TARGET_LINK_LIBRARIES( OrcaImageViewer ${OPENCV_LIBS} OrcaImage)

        GENERATE_CONFIG_FILE( imageviewer.def )

      ELSE( OPENCV_FOUND )
        MESSAGE( STATUS "ImageViewer will not be built -- OpenCv (0.9.9) not found." )
        SET ( BUILD_IMAGE_VIEWER 0)
      ENDIF( OPENCV_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( NOT WIN32 AND BUILD_IMAGE_VIEWER )
