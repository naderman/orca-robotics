OPTION( BUILD_IMAGE_VIEWER "Try to build Image Viewer" ON )

IF ( NOT OS_WIN AND BUILD_IMAGE_VIEWER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

  # this is the only implemenation so far
  SET ( OPENCV_IMPLEMENTATION 1 )

  IF ( OPENCV_IMPLEMENTATION )
      ADD_DEFINITIONS( -DOPENCV_IMPLEMENTATION )

      IF( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )

        IF ( TRICLOPS_FOUND )
          MESSAGE( STATUS "    The Triclops libraries have been found - you will be able to view images from a digiclops camera")
        ELSE ( TRICLOPS_FOUND )
          MESSAGE( STATUS "    The Triclops libraries have not been found - you will not be able to view images from a digiclops camera")
        ENDIF ( TRICLOPS_FOUND )
    
        FIND_COMPONENT_SOURCES( COMP_SRCS )
        SET ( COMP_DEPS ${OPENCV_LIBS} OrcaImage ) 
        
        # build stand-alone component  
        GLOBAL_ADD_EXECUTABLE( imageviewer ${COMP_SRCS} main )
        TARGET_LINK_LIBRARIES( imageviewer ${COMP_DEPS} )
        
        # build IceBox service
        IF ( BUILD_SERVICES )
            GLOBAL_ADD_LIBRARY( OrcaImageViewer SHARED ${COMP_SRCS} service )
            TARGET_LINK_LIBRARIES( OrcaImageViewer ${COMP_DEPS} ) 
        ENDIF ( BUILD_SERVICES )

        GENERATE_CONFIG_FILE( imageviewer.def )

      ELSE( OPENCV_FOUND )
        MESSAGE( STATUS "ImageViewer will not be built -- OpenCv (0.9.9) not found." )
        SET ( BUILD_IMAGE_VIEWER 0)
      ENDIF( OPENCV_FOUND )  

  ENDIF( OPENCV_IMPLEMENTATION )

ENDIF ( NOT OS_WIN AND BUILD_IMAGE_VIEWER )
