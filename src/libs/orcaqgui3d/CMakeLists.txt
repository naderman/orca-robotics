IF ( NOT WIN32 AND QT4_FOUND AND GLUT_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    
    #
    # A list of files to MOC
    #
    SET( MOC_HEADERS
        worldview.h
    )
        
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui3d SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( OrcaQGui3d
      OrcaQCompModel 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTCORE_LIBRARY}
      ${QT_QTOPENGL_LIBRARY}
    )

    INSTALL_FILES( /include/orcaqgui3d .*\\.h$ )

ELSE  ( NOT WIN32 AND QT4_FOUND AND GLUT_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: libOrcaQGui3d -- QT4 or GLUT not found")

ENDIF ( NOT WIN32 AND QT4_FOUND AND GLUT_FOUND )
