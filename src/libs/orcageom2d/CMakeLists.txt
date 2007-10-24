# IF ( NOT OS_QNX )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    IF ( NOT OS_WIN )
		GLOBAL_ADD_LIBRARY( OrcaGeom2d SHARED ${SRCS} )
	ELSE ( NOT OS_WIN )
		GLOBAL_ADD_LIBRARY( OrcaGeom2d STATIC ${SRCS} )
	ENDIF ( NOT OS_WIN )

    GLOBAL_ADD_LIBRARY( OrcaGeom2d SHARED ${SRCS} )
    
    INSTALL_TARGETS( /lib OrcaGeom2d )
    INSTALL_FILES( /include/orcageom2d .*\\.h$ )
    
    IF ( BUILD_TESTS ) 
        ADD_SUBDIRECTORY( test )
    ENDIF ( BUILD_TESTS )
 
# ENDIF ( NOT OS_QNX )