include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

link_libraries( OrcaLog )

add_executable( logtest logtest.cpp )
GBX_ADD_TEST( OrcaLog_Test logtest )

add_executable( mastertest mastertest.cpp )
GBX_ADD_TEST( OrcaLog_LogReplayMaster mastertest "no-config-file-needed" )
