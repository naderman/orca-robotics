###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

MESSAGE( STATUS "== GLOBAL DEPENDENCY CHECK ==" )

INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

# Can we use pkg-config?
INCLUDE(${ORCA_CMAKE_DIR}/FindPkgConfig.cmake)

# Check for Qt
INCLUDE(${CMAKE_ROOT}/Modules/FindQt4.cmake)
# we do NOT want 4.0.x
IF( QTVERSION MATCHES "4.1.*")
    SET( QT4_FOUND FALSE )
ENDIF( QTVERSION MATCHES "4.1.*")
IF( QT4_FOUND )
    MESSAGE( STATUS "Looking for Qt4 >= 4.2 - found")
#    MESSAGE( STATUS "DEBUG:   version: ${QTVERSION}" )
#    MESSAGE( STATUS "DEBUG:   Core library: ${QT_QTCORE_LIBRARY}" )
#    MESSAGE( STATUS "DEBUG:   GUI library: ${QT_QTGUI_LIBRARY}" )
#    MESSAGE( STATUS "DEBUG:   Includes in ${QT_INCLUDE_DIR}")
#    MESSAGE( STATUS "DEBUG:   Libraries in ${QT_LIBRARY_DIR}")
#    MESSAGE( STATUS "DEBUG:   Libraries ${QT_LIBRARIES}" )
ELSE ( QT4_FOUND )
    MESSAGE( STATUS "Looking for Qt4 >= 4.2 - not found")
ENDIF( QT4_FOUND )
# some libraries require v.4.2
# SET( QT42_FOUND FALSE )
# IF( QTVERSION MATCHES "4.2.*")
#     SET( QT42_FOUND TRUE )
#     MESSAGE( STATUS "Looking for Qt4 >= 4.2 - found")
# ENDIF( QTVERSION MATCHES "4.2.*")

# Check for GLUT
CHECK_INCLUDE_FILE( GL/glut.h GLUT_FOUND )

# Check for lib3ds
CHECK_INCLUDE_FILE( lib3ds/node.h LIB3DS_FOUND )

# Check for OpenSceneGraph
CHECK_INCLUDE_FILE_CXX( osg/Node OPENSCENEGRAPH_FOUND )

# Check for Java installation
INCLUDE(${CMAKE_ROOT}/Modules/FindJava.cmake)
# MESSAGE( STATUS "DEBUG:   Java runtime ${JAVA_RUNTIME}" )
# MESSAGE( STATUS "DEBUG:   Java archive ${JAVA_ARCHIVE}" )
# MESSAGE( STATUS "DEBUG:   Java compiler ${JAVA_COMPILE}" )
IF( JAVA_RUNTIME )
    SET( JAVA_FOUND 1 )
ENDIF( JAVA_RUNTIME )
ASSERT( JAVA_FOUND "Looking for Java - not found" "Looking for Java - found" )

# Check for Python interpreter installation
INCLUDE(${CMAKE_ROOT}/Modules/FindPythonInterp.cmake)
ASSERT( PYTHONINTERP_FOUND "Looking for the Python interpreter - not found" "Looking for the Python interpreter - found" )

# Look for firewire headers (for firewire cameras)
CHECK_INCLUDE_FILE_CXX( libdc1394/dc1394_control.h 1394_FOUND )

# Look for video-for-linux (for usb cameras).
CHECK_INCLUDE_FILE( linux/videodev.h V4L_FOUND )
CHECK_INCLUDE_FILE( linux/videodev2.h V4L2_FOUND )
