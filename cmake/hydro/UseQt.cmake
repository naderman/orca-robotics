#
# Include and link against Qt core and gui libs
#
include_directories( ${QT_INCLUDES} )

add_definitions( ${QT_DEFINITIONS} )

link_libraries( ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
