IF ( OS_LINUX )

    # alexm: this seems like a big dependency for wifi tools.
    # not sure what is actually needed.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaWifi SHARED ${SRCS} )
    
    INSTALL_FILES( /include/orcawifi .*\\.h$ )
    
    IF ( BUILD_TESTS ) 
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF ( OS_LINUX )


