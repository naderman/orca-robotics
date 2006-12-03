IF ( NOT WIN32 AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}        
        ${ICE_HOME}/include
        ${PROJECT_BINARY_DIR}/src/interfaces/cpp   # these include files are auto generated!
        ${PROJECT_SOURCE_DIR}/src/libs
    )
        
    LINK_DIRECTORIES(
        ${ICE_HOME}/lib
    )
    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )

    #
    # A list of files to MOC
    #
    SET( MOC_HEADERS
      ocmmodel.h
      ocmview.h
      ocmdelegate.h
      modelhandler.h
    )
        
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQCompModel SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQCompModel OrcaCompModel IceGrid
        ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY}
    )
    
    INSTALL_FILES( /include/orcaqcm .*\\.h$ )

ELSE  ( NOT WIN32 AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQCompModel -- Qt4 not found")

ENDIF ( NOT WIN32 AND QT4_FOUND )
