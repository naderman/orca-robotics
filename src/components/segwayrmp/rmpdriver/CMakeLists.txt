# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpDriver SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpDriver )


#//**??** Duncan sort this out to do a proper conditional addition!
IF ( DUNCAN_CAN_TEST )
   ADD_SUBDIRECTORY ( can )
ENDIF ( DUNCAN_CAN_TEST ) 

IF ( FTD2XX_FOUND )

  ADD_SUBDIRECTORY( usb )
  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/usb )
  TARGET_LINK_LIBRARIES( OrcaSegwayRmpDriver OrcaSegwayRmpUsb ftd2xx usb )

ENDIF ( FTD2XX_FOUND )

