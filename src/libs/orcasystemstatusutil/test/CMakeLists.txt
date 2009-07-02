include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )

link_libraries( ${LIB_NAME} )

add_executable( utiltest utiltest.cpp )
GBX_ADD_TEST( ${LIB_NAME}_UtilTest utiltest )
