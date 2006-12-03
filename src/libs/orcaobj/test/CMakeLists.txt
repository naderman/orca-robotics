INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

ADD_EXECUTABLE( nametest nametest.cpp )
TARGET_LINK_LIBRARIES( nametest OrcaObjects )
ADD_TEST( ObjectTest nametest )
MESSAGE( STATUS "Planning to Build Test     : ObjectTest [libOrcaObjects]")

ADD_EXECUTABLE( timetest timetest.cpp )
TARGET_LINK_LIBRARIES( timetest OrcaObjects )
ADD_TEST( TimeTest timetest )
MESSAGE( STATUS "Planning to Build Test     : TimeTest [libOrcaObjects]")
