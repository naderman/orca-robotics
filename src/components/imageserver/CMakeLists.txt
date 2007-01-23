IF ( NOT OS_WIN )

OPTION( BUILD_IMAGE_SERVER "Try to build Image Server" ON )

IF ( BUILD_IMAGE_SERVER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    #
    # checks for libraries and drivers
    #
    
    # low level camera driver checks
    IF ( 1394_FOUND )
        SET( 1394_LIBS raw1394 dc1394_control )
    ELSE ( 1394_FOUND )
        MESSAGE( STATUS "    libdc1394 not found: you will not be able to use firewire cameras" )
    ENDIF ( 1394_FOUND )
    
    IF ( NOT V4L_FOUND )
        MESSAGE ( STATUS "    video4linux not found: you will not be able to use usb cameras" )
    ENDIF ( NOT V4L_FOUND )
    
    # opencv image grabber checks
    IF ( OPENCV_FOUND )
        MESSAGE( STATUS "    OpenCv Image Grabber (>= OpenCv version 0.9.9)  - can be built" )
    ELSE ( OPENCV_FOUND )
        MESSAGE( STATUS "    OpenCv Image Grabber (>= OpenCv version 0.9.9)  - cannot be built" )
    ENDIF ( OPENCV_FOUND )
    
    # digiclops image grabber library checks
    IF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
        MESSAGE( STATUS "    The Digiclops Image Grabber - can be built" )
    ELSE ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
        MESSAGE( STATUS "    The Digiclops Image Grabber - cannot be built" )
    ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
    
    # automatically generate #ifdef's for component.cpp
    CONFIGURE_FILE( configimageserver.h.cmake ${PROJECT_SOURCE_DIR}/src/components/imageserver/configimageserver.h )
    
    #
    # orca drivers and image grabbers to be built
    # 
    
    # these can always be built
    SET ( DRIVER_SRCS driver fakedriver )
    
    IF ( V4L_FOUND OR 1394_FOUND )
        IF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
        
        SET ( MONO_DRIVER_SRCS monodriver)
    
        ADD_SUBDIRECTORY( imagegrabber )
    
        ELSE ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
        MESSAGE( STATUS "     Image grabbing libraries cannot be found... only the fake driver can be built." ) 
        MESSAGE( STATUS "     Please install opencv for monocular cameras and/or the digiclops libraries for the trinocular digiclops camera."  )
        # SET ( DRIVER_SRCS driver fakedriver )
        ENDIF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
    
    ELSE ( V4L_FOUND OR 1394_FOUND )
        MESSAGE( STATUS "     Image grabbing drivers cannot be found... only the fake driver can be built." ) 
        MESSAGE( STATUS "     Please install Video4Linux for usb cameras and/or libdc1394 for firewire cameras"  ) 
        #SET ( DRIVER_SRCS fakedriver )
    
    ENDIF ( V4L_FOUND OR 1394_FOUND )
    
    LINK_DIRECTORIES( imagegrabber )
    SET( DEP_LIBS OrcaImage )
    
    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        SET( DEP_LIBS ${DEP_LIBS} ${OPENCV_LIBS} OrcaImageGrabber OrcaImage )
    ENDIF ( OPENCV_FOUND )
    
    IF ( DIGICLOPS_AND_TRICLOPS_FOUND )
        INCLUDE_DIRECTORIES( /usr/local/include/digiclops /usr/local/include/triclops)
        SET( DEP_LIBS ${DEP_LIBS} ${OPENCV_LIBS} digiclops triclops OrcaImageGrabber )
    ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS cameraI component conversions mainloop ${DRIVER_SRCS} ${MONO_DRIVER_SRCS} )
    
    GLOBAL_ADD_EXECUTABLE( imageserver main ${COMMON_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaImageServer SHARED service ${COMMON_SRCS} )

    TARGET_LINK_LIBRARIES( imageserver ${DEP_LIBS} )
    TARGET_LINK_LIBRARIES( OrcaImageServer ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( imageserver.def )

ENDIF ( BUILD_IMAGE_SERVER )
ENDIF ( NOT OS_WIN )
