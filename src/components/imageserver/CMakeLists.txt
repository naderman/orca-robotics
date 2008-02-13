#
# THIS COMPONENT IS HAS NOT BEEN UPDATED IN A WHILE
# DO NOT USE IT A TEMPLATE FOR NEW DEVELOPMENT
#

SET_COMPONENT_NAMES ( ImageServer )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} OFF )
REQUIRE_VAR( build EXE ${APP_NAME} ORCA_OS_LINUX "Only Linux is supported" )

SET( dep_libs OrcaIfaceImpl )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)

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
    SET ( DRIVER_srcs driver fakedriver )
    
    IF ( HAVE_CAMERA_HW_LIB )
        IF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
        
            SET ( MONO_DRIVER_srcs monodriver)
    
            # ADD_SUBDIRECTORY( imagegrabber )
    
        ELSE ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
            MESSAGE( STATUS "     Image grabbing libraries CANNOT be found... only the fake driver can be built." ) 
            MESSAGE( STATUS "     Please install opencv for monocular cameras and/or the digiclops libraries for the trinocular digiclops camera."  )
        ENDIF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
    
    ELSE ( HAVE_CAMERA_HW_LIB )
        MESSAGE( STATUS "     Image grabbing drivers CANNOT be found... only the fake driver can be built." ) 
        MESSAGE( STATUS "     Please install Video4Linux for usb cameras and/or libdc1394 for firewire cameras"  ) 
    ENDIF ( HAVE_CAMERA_HW_LIB )
    
    IF ( OPENCV_FOUND AND HAVE_CAMERA_HW_LIB )
        ADD_DEFINITIONS( -Dbuild_OPENCV_GRABBER )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        SET( dep_libs ${dep_libs} ${OPENCV_LIBS} OrcaImageGrabber OrcaImage )
    ENDIF ( OPENCV_FOUND AND HAVE_CAMERA_HW_LIB )
    
    # check for digiclops image grabbing libraries
    FIND_LIBRARY( DIGICLOPS_FOUND NAMES digiclops PATHS /usr/lib /usr/local/lib )
    ASSERT( DIGICLOPS_FOUND "Looking for Digiclops - not found" "Looking for Digiclops - found" )
    
    FIND_LIBRARY( TRICLOPS_FOUND NAMES triclops PATHS /usr/lib /usr/local/lib )
    ASSERT( TRICLOPS_FOUND "Looking for Triclops - not found" "Looking for Triclops - found" )
    
    # both digiclops and triclops libraries need to be used
    IF ( DIGICLOPS_FOUND AND TRICLOPS_FOUND ) 
        SET ( DIGICLOPS_AND_TRICLOPS_FOUND 1 )
    ENDIF ( DIGICLOPS_FOUND AND TRICLOPS_FOUND ) 

    IF ( DIGICLOPS_AND_TRICLOPS_FOUND )
        ADD_DEFINITIONS( -Dbuild_DIGICLOPS_GRABBER )
        INCLUDE_DIRECTORIES( /usr/local/include/digiclops /usr/local/include/triclops)
        SET( dep_libs ${dep_libs} ${OPENCV_LIBS} digiclops triclops OrcaImage OrcaImageGrabber )
    ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND )
    

    SET( srcs component mainthread ${DRIVER_srcs} ${MONO_DRIVER_srcs} )
    
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build an IceBox service
    IF ( ORCA_BUILD_SERVICES )
        WRAP_COMPONENT_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} )    
    ENDIF ( ORCA_BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
