INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/segwayrmp )

# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpUsb SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpUsb )
TARGET_LINK_LIBRARIES( OrcaSegwayRmpUsb OrcaSegwayRmpUsbFtdi )

ADD_SUBDIRECTORY( usbftdi )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
