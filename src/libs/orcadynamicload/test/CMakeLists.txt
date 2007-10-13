INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

ADD_LIBRARY( OrcaDllTestOne SHARED myimplementation.cpp )
ADD_LIBRARY( OrcaDllTestMulti SHARED multiimplementation.cpp )

ADD_EXECUTABLE( dlltestone dlltestone.cpp )
TARGET_LINK_LIBRARIES( dlltestone OrcaDynamicLoad )
ADD_TEST( DllTestOne           dlltestone )
MESSAGE( STATUS "Planning to Build Test     : DllTestOne [libOrcaDynamicLoad]")

ADD_EXECUTABLE( dlltestmulti dlltestmulti.cpp )
TARGET_LINK_LIBRARIES( dlltestmulti OrcaDynamicLoad )
ADD_TEST( DllTestMulti           dlltestmulti )
MESSAGE( STATUS "Planning to Build Test     : DllTestMulti [libOrcaDynamicLoad]")
