INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaLog )

ADD_EXECUTABLE( logtest logtest.cpp )
GBX_ADD_TEST( OrcaLog_Test logtest )

ADD_EXECUTABLE( mastertest mastertest.cpp )
GBX_ADD_TEST( OrcaLog_LogReplayMaster mastertest "no-config-file-needed" )
