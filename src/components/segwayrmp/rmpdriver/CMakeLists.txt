# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpDriver SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpDriver )

# Look for libpcan.so (the Peak CAN library)
INCLUDE (${ORCA_CMAKE_DIR}/FindPeakCan.cmake)

IF ( BUILD_PEAKCAN AND PEAKCAN_FOUND )
  
  ADD_SUBDIRECTORY ( can )
  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/can )
  TARGET_LINK_LIBRARIES( OrcaSegwayRmpDriver OrcaSegwayRmpCan pcan )

ENDIF ( BUILD_PEAKCAN  AND PEAKCAN_FOUND )


# Look for libftd2xx.so (a high level USB library)
INCLUDE (${ORCA_CMAKE_DIR}/FindFtd2xx.cmake)

IF ( FTD2XX_FOUND )

  ADD_SUBDIRECTORY( usb )
  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/usb )
  TARGET_LINK_LIBRARIES( OrcaSegwayRmpDriver OrcaSegwayRmpUsb ftd2xx usb )

ENDIF ( FTD2XX_FOUND )

