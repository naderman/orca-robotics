INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

FILE( GLOB SRCS *.cpp )

IF ( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( OrcaCompModel SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaCompModel OrcaMisc IceGrid )
ELSE ( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( OrcaCompModel STATIC ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaCompModel OrcaMisc
                optimized IceGrid debug IceGridd
#               optimized Glacier2 debug Glacier2d
               )
ENDIF ( NOT OS_WIN )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaCompModel OrcaInterfaces )

INSTALL_FILES( /include/orcacm .*\\.h$ )
