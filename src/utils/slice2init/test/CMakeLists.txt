GBX_ADD_TEST( Slice2InitTest ${CMAKE_CURRENT_BINARY_DIR}/../slice2init ${CMAKE_CURRENT_SOURCE_DIR}/dating.ice  )

INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceInit.cmake )

ADD_EXECUTABLE( inittest inittest.cpp )
GBX_ADD_TEST( Slice2InitInitTest inittest )
