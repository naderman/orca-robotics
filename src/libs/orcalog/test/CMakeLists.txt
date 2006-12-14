INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( logtest logtest.cpp )
TARGET_LINK_LIBRARIES( logtest OrcaLog )
ADD_TEST( LibOrcaLogTest logtest )
MESSAGE( STATUS "Planning to Build Test      : LibOrcaLogTest [libOrcaLog]")

ADD_EXECUTABLE( mastertest mastertest.cpp )
TARGET_LINK_LIBRARIES( mastertest OrcaLog )
ADD_TEST( LogReplayMasterTest mastertest )
MESSAGE( STATUS "Planning to Build Test      : LogReplayMasterTest [libOrcaLog]")
