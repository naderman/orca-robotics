IF ( NOT OS_WIN AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    SET( MOC_HEADERS
        guielementmodel.h
        guielementview.h
        mainwin.h
        regselectview.h
        shortcutaction.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQGui OrcaQt OrcaQCompModel )
    
    INSTALL_FILES( /include/orcaqgui .*\\.h$ )

ELSE  ( NOT OS_WIN AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: libOrcaQGui -- QT4 not found")

ENDIF ( NOT OS_WIN AND QT4_FOUND )
