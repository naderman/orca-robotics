###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

MESSAGE( STATUS "== GLOBAL DEPENDENCY CHECK ==" )

INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

#
# Include local macro definitions
#
INCLUDE( ${ORCA_CMAKE_DIR}/orca_macros.cmake )

# Can we use pkg-config?
INCLUDE (${ORCA_CMAKE_DIR}/FindPkgConfig.cmake)

# Look for Player
INCLUDE (${ORCA_CMAKE_DIR}/FindPlayer2.cmake)

# Look for libftd2xx.so (a high level USB library)
INCLUDE (${ORCA_CMAKE_DIR}/FindFtd2xx.cmake)

# Look for libpcan.so (the Peak CAN library)
INCLUDE (${ORCA_CMAKE_DIR}/FindPeakCan.cmake)

# Check for Qt
INCLUDE (${CMAKE_ROOT}/Modules/FindQt4.cmake)
# we do NOT want 4.0.x
IF ( QTVERSION MATCHES "4.1.*")
    SET ( QT4_FOUND FALSE )
ENDIF ( QTVERSION MATCHES "4.1.*")
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
ENDIF ( QT4_FOUND )
# some libraries require v.4.2
# SET ( QT42_FOUND FALSE )
# IF ( QTVERSION MATCHES "4.2.*")
#     SET ( QT42_FOUND TRUE )
#     MESSAGE( STATUS "Looking for Qt4 >= 4.2 - found")
# ENDIF ( QTVERSION MATCHES "4.2.*")

# Check for GLUT
CHECK_INCLUDE_FILE( GL/glut.h GLUT_FOUND )

# Check for lib3ds
CHECK_INCLUDE_FILE( lib3ds/node.h LIB3DS_FOUND )

# Check for Java installation
INCLUDE (${CMAKE_ROOT}/Modules/FindJava.cmake)
# MESSAGE( STATUS "DEBUG:   Java runtime ${JAVA_RUNTIME}" )
# MESSAGE( STATUS "DEBUG:   Java archive ${JAVA_ARCHIVE}" )
# MESSAGE( STATUS "DEBUG:   Java compiler ${JAVA_COMPILE}" )
IF ( JAVA_RUNTIME )
    SET ( JAVA_FOUND 1 )
ENDIF ( JAVA_RUNTIME )
ASSERT( JAVA_FOUND "Looking for Java - not found" "Looking for Java - found" )

# Check for Python interpreter installation
INCLUDE (${CMAKE_ROOT}/Modules/FindPythonInterp.cmake)
ASSERT( PYTHONINTERP_FOUND "Looking for the Python interpreter - not found" "Looking for the Python interpreter - found" )

# # Check for OpenCV
# FIND_LIBRARY( OPENCV_FOUND NAMES opencv cv cv0.9 PATHS /usr/lib /usr/local/lib )
# ASSERT( OPENCV_FOUND "Looking for OpenCV - not found" "Looking for OpenCV - found" )
# 
# # Check for CVAux
# FIND_LIBRARY( CVAUX_FOUND NAMES cvaux cvaux0.9 PATHS /usr/lib /usr/local/lib )
# ASSERT( CVAUX_FOUND "Looking for CVAux - not found" "Looking for CVAux - found" )
# 
# # Check for HighGUI
# FIND_LIBRARY( HIGHGUI_FOUND NAMES highgui highgui0.9 highgui0.9.7 PATHS /usr/lib /usr/local/lib )
# ASSERT( HIGHGUI_FOUND "Looking for HighGUI - not found" "Looking for HighGUI - found" )

# Check for OpenCV
INCLUDE (${ORCA_CMAKE_DIR}/FindOpencv.cmake)

# check for digiclops image grabbing libraries
FIND_LIBRARY( DIGICLOPS_FOUND NAMES digiclops PATHS /usr/lib /usr/local/lib )
ASSERT( DIGICLOPS_FOUND "Looking for Digiclops - not found" "Looking for Digiclops - found" )

FIND_LIBRARY( TRICLOPS_FOUND NAMES triclops PATHS /usr/lib /usr/local/lib )
ASSERT( TRICLOPS_FOUND "Looking for Triclops - not found" "Looking for Triclops - found" )

# both digiclops and triclops libraries need to be used
IF ( DIGICLOPS_FOUND AND TRICLOPS_FOUND ) 
  SET ( DIGICLOPS_AND_TRICLOPS_FOUND 1 )
ENDIF ( DIGICLOPS_FOUND AND TRICLOPS_FOUND ) 

# Look for firewire headers (for firewire cameras)
CHECK_INCLUDE_FILE_CXX( libdc1394/dc1394_control.h 1394_FOUND )

# Look for video-for-linux (for usb cameras).
CHECK_INCLUDE_FILE( linux/videodev.h V4L_FOUND )
CHECK_INCLUDE_FILE( linux/videodev2.h V4L2_FOUND )

INCLUDE( ${CMAKE_ROOT}/Modules/FindCurses.cmake )
IF ( CURSES_INCLUDE_DIR )
    MESSAGE( STATUS "Looking for libncurses - found")
    SET( CURSES_FOUND 1 CACHE INTERNAL "libncurses" )
ELSE ( CURSES_INCLUDE_DIR )
    MESSAGE( STATUS "Looking for libncurses - not found")
    SET( CURSES_FOUND 0 CACHE INTERNAL "libncurses" )
ENDIF ( CURSES_INCLUDE_DIR )

# Check for ZLIB
INCLUDE (${CMAKE_ROOT}/Modules/FindZLIB.cmake)
SET( HAVE_ZLIB ${ZLIB_FOUND} )

# Look for Boost ublas library
# INCLUDE (${ORCA_CMAKE_DIR}/FindBoostUblas.cmake)

# Look for liblapack library -- linear algebra package
# INCLUDE (${ORCA_CMAKE_DIR}/FindLapack.cmake)
