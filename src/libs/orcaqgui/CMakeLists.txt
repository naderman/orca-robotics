IF ( NOT WIN32 AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )

    #
    # A list of files to MOC
    #
    SET( MOC_HEADERS
        mainwin.h
        regselectview.h
        guielementmodel.h
        guielementview.h
    )
        
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQGui
      OrcaQCompModel 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTCORE_LIBRARY}
    )
    
    INSTALL_FILES( /include/orcaqgui .*\\.h$ )

ELSE  ( NOT WIN32 AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: libOrcaQGui -- QT4 not found")

ENDIF ( NOT WIN32 AND QT4_FOUND )
