INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

# Glob all source files
FILE( GLOB SRCS *.cpp )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/segwayrmp/usb )

ADD_LIBRARY( OrcaSegwayRmpUsbFtdi SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaSegwayRmpUsbFtdi )
