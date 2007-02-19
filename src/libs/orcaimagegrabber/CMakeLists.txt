IF( OS_LINUX )

INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

ADD_DEFINITIONS( -Wall )

#
# checks for libraries and drivers
#

# opencv image grabber checks
IF ( OPENCV_FOUND )
    MESSAGE( STATUS "    OpenCv Image Grabber (>= OpenCv version 0.9.9)  - can be built" )

    INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
    SET( SRCS cvgrabber )

ELSE ( OPENCV_FOUND )
    MESSAGE( STATUS "    OpenCv Image Grabber (>= OpenCv version 0.9.9)  - cannot be built" )
ENDIF ( OPENCV_FOUND )

# digiclops image grabber library checks
IF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
    MESSAGE( STATUS "    The Digiclops Image Grabber - can be built" )

    INCLUDE_DIRECTORIES(
	/usr/local/include/digiclops
	/usr/local/include/triclops
    )
    SET( DIGICLOPS_LIBS digiclops triclops )
    SET( SRCS ${SRCS} digiclopsgrabber )

ELSE ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
    MESSAGE( STATUS "    The Digiclops Image Grabber - cannot be built" )
ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 


# only build the image grabbers if the correct libraries have been found
# IF ( DIGICLOPS_AND_TRICLOPS_FOUND )
# ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND )

# IF ( OPENCV_FOUND )
# ENDIF ( OPENCV_FOUND )

IF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
    GLOBAL_ADD_LIBRARY( OrcaImageGrabber SHARED ${SRCS} )

    TARGET_LINK_LIBRARIES( OrcaImageGrabber ${OPENCV_LIBS} ${DIGICLOPS_LIBS} )

    INSTALL_FILES( /include/orcaimagegrabber .*\\.h$ )
ENDIF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )

ENDIF( OS_LINUX )