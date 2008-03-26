SET ( MODULE_NAME def_tools )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${MODULE_NAME} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    LINK_LIBRARIES( OrcaDef )
    
    GBX_ADD_EXECUTABLE( def2cfg def2cfg.cpp )
    
    GBX_ADD_EXECUTABLE( def2xml def2xml.cpp )
    
    GBX_ADD_EXECUTABLE( def2xmltemplate def2xmltemplate.cpp )
    
    GBX_ADD_EXECUTABLE( def2definverted def2definverted.cpp )

ENDIF ( build )

# SET ( not_os_win NOT GBX_OS_WIN )
# GBX_REQUIRE_VAR( build EXE ${MODULE_NAME} not_os_win "Windows OS is not supported" )
# GBX_REQUIRE_VAR( build EXE ${MODULE_NAME} QT4_FOUND "Qt4 not found" )
# 
# IF ( build )
#     INCLUDE_DIRECTORIES( ${QT_INCLUDES} )    
#     ADD_DEFINITIONS( ${QT_DEFINITIONS} )
#     GBX_ADD_EXECUTABLE( def2uml def2uml.cpp )
#     TARGET_LINK_LIBRARIES( def2uml OrcaQUml ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
# ENDIF ( build )
