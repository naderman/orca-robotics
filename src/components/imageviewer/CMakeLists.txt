OPTION( BUILD_IMAGE_VIEWER "Try to build Image Viewer" ON )

IF ( NOT WIN32 AND BUILD_IMAGE_VIEWER )

  INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

  # this is the only implemenation so far
  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )
      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      IF( OPENCV7_FOUND )
        MESSAGE( "    Using new opencv (>= 0.9.7)" )
        MESSAGE( "    Make sure this version is from cvs and not the release" )
        INCLUDE_DIRECTORIES( ${OPENCV7_INCLUDE_DIRS} )

        IF ( TRICLOPS_FOUND )
          MESSAGE("    The Triclops libraries have been found - you will be able to view images from a digiclops camera")
        ELSE ( TRICLOPS_FOUND )
          MESSAGE("    The Triclops libraries have not been found - you will not be able to view images from a digiclops camera")
        ENDIF ( TRICLOPS_FOUND )

        # don't include parts of component.cpp
        CONFIGURE_FILE( configimageviewer.h.cmake ${PROJECT_SOURCE_DIR}/src/components/imageviewer/configimageviewer.h )

        # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
        SET( COMMON_SRCS cameraconsumerI component imagehandler )

        # Files for the stand-alone application
        SET( COMP_SRCS main ${COMMON_SRCS} )
        
        # Files for the icebox service
        SET( SERVICE_SRCS service ${COMMON_SRCS} )

        GLOBAL_ADD_EXECUTABLE( imageviewer ${COMP_SRCS} )

        # build an IceBox service
        ADD_LIBRARY( OrcaImageViewer SHARED ${SERVICE_SRCS} )

        IF ( TRICLOPS_FOUND )
          INCLUDE_DIRECTORIES( /usr/local/include/triclops )
          TARGET_LINK_LIBRARIES( imageviewer ${OPENCV7_LIBS} triclops)

          # important to let linker know about this inter-library dependency
          # otherwise IceBox fails to load the service
          TARGET_LINK_LIBRARIES( OrcaImageViewer ${OPENCV7_LIBS} triclops pnmutils)

        ELSE (TRICLOPS_FOUND )
          TARGET_LINK_LIBRARIES( imageviewer ${OPENCV7_LIBS} )
          TARGET_LINK_LIBRARIES( OrcaImageViewer ${OPENCV7_LIBS} )

        ENDIF ( TRICLOPS_FOUND )


        INSTALL_TARGETS( /lib OrcaImageViewer )

        GENERATE_FROM_DEF( imageviewer.def )

      ELSE( OPENCV7_FOUND )
        MESSAGE( STATUS "ImageViewer will not be built -- OpenCv (0.9.7) not found." )
        SET ( BUILD_IMAGE_VIEWER 0)
      ENDIF( OPENCV7_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( NOT WIN32 AND BUILD_IMAGE_VIEWER )
