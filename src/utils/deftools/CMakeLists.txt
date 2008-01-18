SET ( MODULE_NAME def_tools )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${MODULE_NAME} ON )

IF ( BUILD )

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

ENDIF ( BUILD )

SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD EXE ${MODULE_NAME} NOT_OS_WIN "Windows OS is not supported" )
REQUIRE_VAR( BUILD EXE ${MODULE_NAME} QT4_FOUND "Qt4 not found" )

IF ( BUILD )
    INCLUDE_DIRECTORIES( ${QT_INCLUDES} )    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    GLOBAL_ADD_EXECUTABLE( def2uml def2uml.cpp )
    TARGET_LINK_LIBRARIES( def2uml OrcaQUml ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
ENDIF ( BUILD )
