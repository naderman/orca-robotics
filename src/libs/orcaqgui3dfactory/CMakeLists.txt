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
    )
        
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui3dFactory SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( OrcaQGui3dFactory
      OrcaQGui
      OrcaQGui3d
      OrcaQCompModel 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTCORE_LIBRARY}
      ${QT_QTOPENGL_LIBRARY}
    )

    IF ( LIB3DS_FOUND )
      ADD_DEFINITIONS( -DHAVE_LIB3DS )
    TARGET_LINK_LIBRARIES ( OrcaQGui3dFactory
      3ds
      )
    ENDIF ( LIB3DS_FOUND )

ELSE  ( NOT WIN32 AND QT4_FOUND AND GLUT_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: libOrcaQGui3d -- QT4 or GLUT not found")

ENDIF ( NOT WIN32 AND QT4_FOUND AND GLUT_FOUND )
