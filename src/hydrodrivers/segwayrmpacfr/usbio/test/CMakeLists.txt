# include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#include_directories( ${PROJECT_SOURCE_DIR}/src/components/segwayrmb/usb )
#add_executable( segtest segtest.cpp )
#target_link_libraries( segtest OrcaSegwayRmpUsb ftd2xx usb )

GBX_ADD_EXECUTABLE( plugtest plugtest.cpp )
target_link_libraries( plugtest ftd2xx usb )
