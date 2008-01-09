SET ( LIB_NAME OrcaImageGrabber )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
REQUIRE_VAR( LIB ${LIB_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )

    # alexm: this seems like a big dependency
    # not sure what is actually needed.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # opencv image grabber checks
    INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
    IF ( OPENCV_FOUND )
        MESSAGE( STATUS "    OpenCv Image Grabber (>= OpenCv version 0.9.9)  - can be built" )
        INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)
        SET( SRCS cvgrabber )
    ELSE ( OPENCV_FOUND )
        MESSAGE( STATUS "    OpenCv Image Grabber (>= OpenCv version 0.9.9)  - CANNOT be built" )
    ENDIF ( OPENCV_FOUND )
    
    # check for digiclops image grabbing libraries
    FIND_LIBRARY( DIGICLOPS_FOUND NAMES digiclops PATHS /usr/lib /usr/local/lib )
    ASSERT( DIGICLOPS_FOUND "Looking for Digiclops - not found" "Looking for Digiclops - found" )
    
    FIND_LIBRARY( TRICLOPS_FOUND NAMES triclops PATHS /usr/lib /usr/local/lib )
    ASSERT( TRICLOPS_FOUND "Looking for Triclops - not found" "Looking for Triclops - found" )
    
    # both digiclops and triclops libraries need to be used
    IF ( DIGICLOPS_FOUND AND TRICLOPS_FOUND ) 
        SET ( DIGICLOPS_AND_TRICLOPS_FOUND 1 )
    ENDIF ( DIGICLOPS_FOUND AND TRICLOPS_FOUND ) 

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
        MESSAGE( STATUS "    The Digiclops Image Grabber - CANNOT be built" )
    ENDIF ( DIGICLOPS_AND_TRICLOPS_FOUND ) 
    
    IF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
        TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OPENCV_LIBS} ${DIGICLOPS_LIBS} )
    ENDIF ( OPENCV_FOUND OR DIGICLOPS_AND_TRICLOPS_FOUND )
    
    # we want this file to install for fake drivers
    INSTALL_FILES( /include/orcaimagegrabber .*\\.h$ )

ENDIF( BUILD )
