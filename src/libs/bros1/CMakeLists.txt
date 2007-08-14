IF ( NOT OS_WIN ) 

    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )
    ADD_DEFINITIONS( "-Wall" )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( Bros1 SHARED ${SRCS} )

    INSTALL_FILES( /include/bros1 .*\\.h$ )

#     IF ( BUILD_TESTS )
#         ADD_SUBDIRECTORY ( test )
#     ENDIF ( BUILD_TESTS )

ENDIF ( NOT OS_WIN ) 
