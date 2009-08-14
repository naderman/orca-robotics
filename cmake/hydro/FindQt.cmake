# Check for Qt
include(${CMAKE_ROOT}/Modules/FindQt4.cmake)
# we do NOT want 4.0.x
if( QTVERSION MATCHES "4.1.*")
    set( QT4_FOUND FALSE )
endif( QTVERSION MATCHES "4.1.*")
if( QT4_FOUND )
    message( STATUS "Looking for Qt4 >= 4.2 - found")
else( QT4_FOUND )
    message( STATUS "Looking for Qt4 >= 4.2 - not found")
endif( QT4_FOUND )
