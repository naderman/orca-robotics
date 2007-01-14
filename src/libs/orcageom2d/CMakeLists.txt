# IF ( NOT OS_QNX )

    INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    FILE( GLOB SRCS *.cpp )
    GLOBAL_ADD_LIBRARY( OrcaGeom2d SHARED ${SRCS} )
    INSTALL_TARGETS( /lib OrcaGeom2d )
    
    IF ( BUILD_TESTS ) 
    ADD_SUBDIRECTORY( test )
    ENDIF ( BUILD_TESTS )
 
# ENDIF ( NOT OS_QNX )