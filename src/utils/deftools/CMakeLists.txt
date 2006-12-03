INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

GLOBAL_ADD_EXECUTABLE( def2cfg def2cfg.cpp )
TARGET_LINK_LIBRARIES( def2cfg OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2xml def2xml.cpp )
TARGET_LINK_LIBRARIES( def2xml OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2xmltemplate def2xmltemplate.cpp )
TARGET_LINK_LIBRARIES( def2xmltemplate OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2definverted def2definverted.cpp )
TARGET_LINK_LIBRARIES( def2definverted OrcaDef )

IF ( NOT WIN32 AND QT42_FOUND )
    INCLUDE_DIRECTORIES( ${QT_INCLUDES} )    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    GLOBAL_ADD_EXECUTABLE( def2uml def2uml.cpp )
    TARGET_LINK_LIBRARIES( def2uml OrcaQUml ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
ELSE  ( NOT WIN32 AND QT42_FOUND )
    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQuml -- Qt 4.2 not found")
ENDIF ( NOT WIN32 AND QT42_FOUND )