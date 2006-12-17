#
# These utilities are not components.
# We do not include the standard component compile options
# to minimize dependencies.
#
ADD_DEFINITIONS( "-Wall" )
INCLUDE_DIRECTORIES( 
	${ICE_HOME}/include
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp 
    ${PROJECT_SOURCE_DIR}/src/libs
    )
LINK_DIRECTORIES( ${ICE_HOME}/lib )
LINK_LIBRARIES( Ice IceUtil OrcaInterfaces OrcaIce OrcaObjects )

GLOBAL_ADD_EXECUTABLE( def2cfg def2cfg.cpp )
TARGET_LINK_LIBRARIES( def2cfg OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2xml def2xml.cpp )
TARGET_LINK_LIBRARIES( def2xml OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2xmltemplate def2xmltemplate.cpp )
TARGET_LINK_LIBRARIES( def2xmltemplate OrcaDef )

GLOBAL_ADD_EXECUTABLE( def2definverted def2definverted.cpp )
TARGET_LINK_LIBRARIES( def2definverted OrcaDef )

IF ( NOT OS_WIN AND QT4_FOUND )
    INCLUDE_DIRECTORIES( ${QT_INCLUDES} )    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    GLOBAL_ADD_EXECUTABLE( def2uml def2uml.cpp )
    TARGET_LINK_LIBRARIES( def2uml OrcaQUml OrcaDef ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
ELSE  ( NOT OS_WIN AND QT4_FOUND )
    MESSAGE( STATUS "Not Planning to Build Utility: def2uml -- Windows or Qt not found")
ENDIF ( NOT OS_WIN AND QT4_FOUND )