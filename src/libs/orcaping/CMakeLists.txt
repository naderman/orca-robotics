IF ( NOT WIN32 ) 

  IF ( NOT LIBOPING_FOUND )
    MESSAGE( STATUS "Not Planning to Build Library: orcaping -- liboping not found")
  ELSE ( NOT LIBOPING_FOUND )
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaPing SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaPing oping )

    INSTALL_FILES( /include/ping .*\\.h$ )

    IF ( BUILD_TESTS )
      ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

  ENDIF ( NOT LIBOPING_FOUND )

ENDIF ( NOT WIN32 ) 
