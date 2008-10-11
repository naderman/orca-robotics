GBX_ADD_TEST( Slice2StringTest ${CMAKE_CURRENT_BINARY_DIR}/../slice2string ${CMAKE_CURRENT_SOURCE_DIR}/dating.ice  )

INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceString.cmake )

ADD_EXECUTABLE( printtest printtest.cpp )
GBX_ADD_TEST( Slice2StringPrintTest printtest )
