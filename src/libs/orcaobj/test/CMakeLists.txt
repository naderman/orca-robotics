INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

ADD_EXECUTABLE( stringtest stringtest.cpp )
TARGET_LINK_LIBRARIES( stringtest OrcaObjects OrcaIce )
ADD_TEST( ObjectStringTest stringtest )
MESSAGE( STATUS "Planning to Build Test      : ObjectStringTest [libOrcaObjects]")

ADD_EXECUTABLE( timetest timetest.cpp )
TARGET_LINK_LIBRARIES( timetest OrcaObjects OrcaIce )
ADD_TEST( TimeTest timetest )
MESSAGE( STATUS "Planning to Build Test      : TimeTest [libOrcaObjects]")

ADD_EXECUTABLE( mathtest mathtest.cpp )
TARGET_LINK_LIBRARIES( mathtest OrcaObjects OrcaIce )
ADD_TEST( MathTest mathtest )
MESSAGE( STATUS "Planning to Build Test      : MathTest [libOrcaObjects]")
