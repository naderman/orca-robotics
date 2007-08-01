GLOBAL_ADD_EXECUTABLE( serialechotest serialechotest.cpp )
TARGET_LINK_LIBRARIES( serialechotest OrcaSerial )

GLOBAL_ADD_EXECUTABLE( locktest locktest.cpp )
TARGET_LINK_LIBRARIES( locktest OrcaSerial )
