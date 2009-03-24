INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../.. )

GBX_ADD_EXECUTABLE( segwaydriverthread segwaydriverthread.cpp )
TARGET_LINK_LIBRARIES( segwaydriverthread SegwayRmpDriverThread )
