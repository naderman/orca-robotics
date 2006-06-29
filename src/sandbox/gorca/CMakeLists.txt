OPTION( BUILD_GORCA "Try to build Gorca -- a system composition utility" OFF )

IF ( QT4_FOUND AND BUILD_GORCA )

    #INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    
    #
    # This gathers all the .cpp files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${ICE_HOME}/include     # remove this dependency!
        ${PROJ_BINARY_DIR}/src/interfaces/cpp   # these include files are auto generated!
	${PROJ_SOURCE_DIR}/src/utils
        ${PROJ_SOURCE_DIR}/src/sandbox
    )
    
    LINK_DIRECTORIES(
        ${ICE_HOME}/lib         # remove this dependency!
        ${PROJ_BINARY_DIR}/src/interfaces/cpp/orca
        ${PROJ_BINARY_DIR}/src/utils/orcaice
        ${PROJ_BINARY_DIR}/src/utils/orcadef
        ${PROJ_BINARY_DIR}/src/sandbox/qorcauml
    )

    ADD_DEFINITIONS( ${QT_DEFINITIONS} -DQT3_SUPPORT -DINSTALL_PREFIX=\\"${CMAKE_INSTALL_PREFIX}\\" -Wall )
    
    #
    # A list of files to MOC
    #
    SET( MOC_HEADERS
        mainwin.h
        qorcalibview.h
        qorcaprojview.h
        qorcaxmledit.h
    )
    
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_EXECUTABLE( gorca ${HUMAN_SRCS} ${MOC_SRCS} )
    
    # TODO! make sure to deal with Debug/Release of IceUtil if converting to Win32
    TARGET_LINK_LIBRARIES( gorca IceUtil QorcaUml OrcaDef OrcaInterfaces OrcaIce
        ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QT3SUPPORT_LIBRARY}
    )

ELSE ( QT4_FOUND AND BUILD_GORCA )

    MESSAGE("Not Planning to Build Utility: Gorca -- Qt4 not found")
    
ENDIF ( QT4_FOUND AND BUILD_GORCA )
