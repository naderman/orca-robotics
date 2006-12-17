IF ( NOT OS_WIN AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaQt SHARED ${HUMAN_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQt )
    
    INSTALL_FILES( /include/orcaqt .*\\.h$ )

ELSE  ( NOT OS_WIN AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQt -- Qt4 not found")

ENDIF ( NOT OS_WIN AND QT4_FOUND )
