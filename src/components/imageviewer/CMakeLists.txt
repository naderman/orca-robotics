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

        FIND_LIBRARY( TRICLOPS_FOUND NAMES triclops PATHS /usr/lib /usr/local/lib )
        IF ( TRICLOPS_FOUND )
          MESSAGE("-- Looking for Triclops - found")
        ELSE ( TRICLOPS_FOUND )
          MESSAGE("-- Looking for Triclops - not found")
        ENDIF ( TRICLOPS_FOUND )

        IF ( TRICLOPS_FOUND )
          LINK_LIBRARIES( triclops pnmutils)
          INCLUDE_DIRECTORIES(
            /usr/local/include/triclops
            )
        ENDIF ( TRICLOPS_FOUND )

        # don't include parts of component.cpp
        CONFIGURE_FILE( configimageviewer.h.cmake ${PROJ_SOURCE_DIR}/src/components/imageviewer/configimageviewer.h )

        # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
        SET( COMMON_SRCS cameraconsumer_i component imagehandler )

        # Files for the stand-alone application
        SET( COMP_SRCS main ${COMMON_SRCS} )
        
        # Files for the icebox service
        SET( SERVICE_SRCS service ${COMMON_SRCS} )

        GLOBAL_ADD_EXECUTABLE( imageviewer ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( imageviewer ${OPENCV7_LIBS} triclops)

        # build an IceBox service
        ADD_LIBRARY( OrcaImageViewer SHARED ${SERVICE_SRCS} )

        # important to let linker know about this inter-library dependency
        # otherwise IceBox fails to load the service
        TARGET_LINK_LIBRARIES( OrcaImageViewer ${OPENCV7_LIBS} triclops pnmutils)

        INSTALL_TARGETS( /lib OrcaImageViewer )

        GENERATE_FROM_DEF( imageviewer.def )

      ELSE( OPENCV7_FOUND )
        MESSAGE( "ImageViewer will not be built.")
        MESSAGE( "     OpenCv (0.9.7) has not been found. You need the cvs version." )
        SET ( BUILD_IMAGE_VIEWER 0)
      ENDIF( OPENCV7_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( BUILD_IMAGE_VIEWER )
