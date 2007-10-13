IF ( NOT OS_WIN ) 

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

    FILE( GLOB SRCS *.cpp )
    GLOBAL_ADD_LIBRARY( Bros1 SHARED ${SRCS} )

    INSTALL_FILES( /include/bros1 .*\\.h$ )

#     IF ( BUILD_TESTS )
#         ADD_SUBDIRECTORY ( test )
#     ENDIF ( BUILD_TESTS )

ENDIF ( NOT OS_WIN ) 
