INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

LINK_LIBRARIES( OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2cfg def2cfg.cpp )

GLOBAL_ADD_EXECUTABLE( def2xml def2xml.cpp )

GLOBAL_ADD_EXECUTABLE( def2xmltemplate def2xmltemplate.cpp )

GLOBAL_ADD_EXECUTABLE( def2definverted def2definverted.cpp )

IF ( NOT OS_WIN AND QT4_FOUND )
    INCLUDE_DIRECTORIES( ${QT_INCLUDES} )    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    GLOBAL_ADD_EXECUTABLE( def2uml def2uml.cpp )
    TARGET_LINK_LIBRARIES( def2uml OrcaQUml ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
ELSE  ( NOT OS_WIN AND QT4_FOUND )
    MESSAGE( STATUS "Not Planning to Build Utility: def2uml -- Windows or Qt not found")
ENDIF ( NOT OS_WIN AND QT4_FOUND )