IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

    FILE( GLOB SRCS *.cpp )
    GLOBAL_ADD_LIBRARY( OrcaDynamicLoad SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaDynamicLoad dl )
    
    INSTALL_FILES( /include/orcadynamicload .*\\.h$ ) 

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( NOT OS_WIN )
