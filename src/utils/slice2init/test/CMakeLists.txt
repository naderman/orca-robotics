GBX_ADD_TEST( Slice2InitTest ${CMAKE_CURRENT_BINARY_DIR}/../slice2init ${CMAKE_CURRENT_SOURCE_DIR}/dating.ice  )

include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceInit.cmake )

add_executable( inittest inittest.cpp )
GBX_ADD_TEST( Slice2InitInitTest inittest )
