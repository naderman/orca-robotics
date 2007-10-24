INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaLog )

ADD_EXECUTABLE( logtest logtest.cpp )
GLOBAL_ADD_TEST( LibOrcaLogTest logtest )

ADD_EXECUTABLE( mastertest mastertest.cpp )
GLOBAL_ADD_TEST( LogReplayMasterTest mastertest )
