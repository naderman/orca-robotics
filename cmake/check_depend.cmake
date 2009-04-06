###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

message( STATUS "== GLOBAL DEPENDENCY CHECK ==" )

include( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )
include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

# Can we use pkg-config?
include(${ORCA_CMAKE_DIR}/FindPkgConfig.cmake)

# Check for Qt
include(${CMAKE_ROOT}/Modules/FindQt4.cmake)
# we do NOT want 4.0.x
if( QTVERSION MATCHES "4.1.*")
    set( QT4_FOUND FALSE )
endif( QTVERSION MATCHES "4.1.*")
if( QT4_FOUND )
    message( STATUS "Looking for Qt4 >= 4.2 - found")
#    message( STATUS "DEBUG:   version: ${QTVERSION}" )
#    message( STATUS "DEBUG:   Core library: ${QT_QTCORE_LIBRARY}" )
#    message( STATUS "DEBUG:   GUI library: ${QT_QTGUI_LIBRARY}" )
#    message( STATUS "DEBUG:   OpenGL library: ${QT_QTOPENGL_LIBRARY}" )
#    message( STATUS "DEBUG:   Includes in ${QT_INCLUDE_DIR}")
#    message( STATUS "DEBUG:   Libraries in ${QT_LIBRARY_DIR}")
#    message( STATUS "DEBUG:   Libraries ${QT_LIBRARIES}" )
else( QT4_FOUND )
    message( STATUS "Looking for Qt4 >= 4.2 - not found")
endif( QT4_FOUND )
# some libraries require v.4.2
# set( QT42_FOUND FALSE )
# if( QTVERSION MATCHES "4.2.*")
#     set( QT42_FOUND TRUE )
#     message( STATUS "Looking for Qt4 >= 4.2 - found")
# endif( QTVERSION MATCHES "4.2.*")

# Check for GLUT
check_include_file( GL/glut.h GLUT_FOUND )

# Check for lib3ds
check_include_file( lib3ds/node.h LIB3DS_FOUND )

# Check for OpenCV
INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)

# Check for OpenSceneGraph
check_include_file_cxx( osg/Node OPENSCENEGRAPH_FOUND )

# Check for potrace
check_include_file( potrace/potracelib.h POTRACE_FOUND )

# Check for Java installation
include(${CMAKE_ROOT}/Modules/FindJava.cmake)
# message( STATUS "DEBUG:   Java runtime ${JAVA_RUNTIME}" )
# message( STATUS "DEBUG:   Java archive ${JAVA_ARCHIVE}" )
# message( STATUS "DEBUG:   Java compiler ${JAVA_COMPILE}" )
if( JAVA_RUNTIME )
    set( JAVA_FOUND 1 )
endif( JAVA_RUNTIME )
ASSERT( JAVA_FOUND "Looking for Java - not found" "Looking for Java - found" )

# Check for Python interpreter installation
include(${CMAKE_ROOT}/Modules/FindPythonInterp.cmake)
ASSERT( PYTHONINTERP_FOUND "Looking for the Python interpreter - not found" "Looking for the Python interpreter - found" )

# Look for firewire headers (for firewire cameras)
check_include_file_cxx( libdc1394/dc1394_control.h 1394_FOUND )

# Look for video-for-linux (for usb cameras).
check_include_file( linux/videodev.h V4L_FOUND )
check_include_file( linux/videodev2.h V4L2_FOUND )
