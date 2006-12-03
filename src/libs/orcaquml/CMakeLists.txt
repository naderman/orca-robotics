IF ( NOT WIN32 AND QT42_FOUND )

    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}        
        ${PROJECT_BINARY_DIR}/src/interfaces/cpp   # these include files are auto generated!
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )

    GLOBAL_ADD_LIBRARY( OrcaQUml SHARED ${HUMAN_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQUml OrcaDef ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

ELSE  ( NOT WIN32 AND QT42_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQuml -- Qt 4.2 not found")

ENDIF ( NOT WIN32 AND QT42_FOUND )
