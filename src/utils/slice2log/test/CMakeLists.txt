GBX_ADD_TEST( Slice2LogTest ${CMAKE_CURRENT_BINARY_DIR}/../slice2log ${CMAKE_CURRENT_SOURCE_DIR}/test.ice  )

INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceLog.cmake )

ADD_EXECUTABLE( logtest logtest.cpp )
GBX_ADD_TEST( Slice2LogLogTest logtest )
