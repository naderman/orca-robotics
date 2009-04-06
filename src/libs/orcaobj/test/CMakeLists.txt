include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )

link_libraries( OrcaObjects )

add_executable( objstringtest stringtest.cpp )
GBX_ADD_TEST( OrcaObjects_StringTest objstringtest )
