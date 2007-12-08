#
# THIS COMPONENT IS HAS NOT BEEN UPDATED IN A WHILE
# DO NOT USE IT A TEMPLATE FOR NEW DEVELOPMENT
#

IF ( OS_LINUX )

OPTION( BUILD_IMAGE_SERVER "Try to build Image Server" ON )

IF ( BUILD_IMAGE_SERVER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # automatically generate #ifdef's for component.cpp
    # CONFIGURE_FILE( configimageserver.h.cmake ${PROJECT_SOURCE_DIR}/src/components/imageserver/configimageserver.h )
    
    # low level camera driver checks
    IF ( 1394_FOUND )
	SET( 1394_LIBS raw1394 dc1394_control )
    ELSE ( 1394_FOUND )
	MESSAGE( STATUS "    libdc1394 not found: you will not be able to use firewire cameras" )
    ENDIF ( 1394_FOUND )
    
    IF ( NOT V4L_FOUND )
	MESSAGE ( STATUS "    video4linux not found: you will not be able to use usb cameras" )
    ENDIF ( NOT V4L_FOUND )

    IF ( V4L_FOUND OR 1394_FOUND )
	SET( HAVE_CAMERA_HW_LIB 1 )
    ENDIF ( V4L_FOUND OR 1394_FOUND )
    #
    # orca drivers and image grabbers to be built
    # 
    
    # these can always be built
    SET ( DRIVER_SRCS driver fakedriver )
    
    IF ( HAVE_CAMERA_HW_LIB )
        IF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
        
            SET ( MONO_DRIVER_SRCS monodriver)
    
            # ADD_SUBDIRECTORY( imagegrabber )
    
        ELSE ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
            MESSAGE( STATUS "     Image grabbing libraries CANNOT be found... only the fake driver can be built." ) 
            MESSAGE( STATUS "     Please install opencv for monocular cameras and/or the digiclops libraries for the trinocular digiclops camera."  )
        ENDIF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
    
    ELSE ( HAVE_CAMERA_HW_LIB )
        MESSAGE( STATUS "     Image grabbing drivers CANNOT be found... only the fake driver can be built." ) 
        MESSAGE( STATUS "     Please install Video4Linux for usb cameras and/or libdc1394 for firewire cameras"  ) 
    ENDIF ( HAVE_CAMERA_HW_LIB )
    
    # LINK_DIRECTORIES( imagegrabber )
    SET( COMP_DEPS OrcaImage OrcaIfaceImpl )
    
    IF ( OPENCV_FOUND AND HAVE_CAMERA_HW_LIB )
        ADD_DEFINITIONS( -DBUILD_OPENCV_GRABBER )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        SET( COMP_DEPS ${COMP_DEPS} ${OPENCV_LIBS} OrcaImageGrabber )
    ENDIF ( OPENCV_FOUND AND HAVE_CAMERA_HW_LIB )
    
    IF ( DIGICLOPS_AND_TRICLOPS_FOUND )
        ADD_DEFINITIONS( -DBUILD_DIGICLOPS_GRABBER )
        INCLUDE_DIRECTORIES( /usr/local/include/digiclops /usr/local/include/triclops)
        SET( COMP_DEPS ${COMP_DEPS} ${OPENCV_LIBS} digiclops triclops OrcaImageGrabber )
    ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND )
    
    # CANNOT use standard macro, because some driver files are optional
    SET( COMMON_SRCS component mainloop ${DRIVER_SRCS} ${MONO_DRIVER_SRCS} )
    
    GLOBAL_ADD_EXECUTABLE( imageserver main ${COMMON_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaImageServer SHARED service ${COMMON_SRCS} )

    TARGET_LINK_LIBRARIES( imageserver ${COMP_DEPS} )
    TARGET_LINK_LIBRARIES( OrcaImageServer ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( imageserver.def )

ENDIF ( BUILD_IMAGE_SERVER )
ENDIF ( OS_LINUX )
