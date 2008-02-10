OPTION( build_GARMIN_GPS "Try to build garmin GPS driver" ON )

IF ( build_GARMIN_GPS )


INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/gps )

FILE( GLOB srcs *.cpp )
ADD_LIBRARY( OrcaGarminGps SHARED ${srcs} )

INSTALL_TARGETS( /lib OrcaGarminGps )

ENDIF(build_GARMIN_GPS)