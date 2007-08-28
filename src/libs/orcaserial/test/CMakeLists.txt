GLOBAL_ADD_EXECUTABLE( serialechotest serialechotest.cpp )
TARGET_LINK_LIBRARIES( serialechotest OrcaSerial )

GLOBAL_ADD_EXECUTABLE( serialloopbacktest serialloopbacktest.cpp )
TARGET_LINK_LIBRARIES( serialloopbacktest OrcaSerial )
