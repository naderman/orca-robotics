IF ( NOT OS_WIN AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )

    SET( MOC_HEADERS
      ocmmodel.h
      ocmview.h
      ocmdelegate.h
      modelhandler.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQCompModel SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQCompModel OrcaCompModel IceGrid )
    
    INSTALL_FILES( /include/orcaqcm .*\\.h$ )

ELSE  ( NOT OS_WIN AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQCompModel -- Qt4 not found")

ENDIF ( NOT OS_WIN AND QT4_FOUND )
