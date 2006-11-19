INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/segwayrmb/usb )

ADD_EXECUTABLE( segtest segtest.cpp )
TARGET_LINK_LIBRARIES( segtest OrcaSegwayRmpUsb ftd2xx usb )
