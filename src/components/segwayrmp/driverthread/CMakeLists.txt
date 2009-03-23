SET( lib_name SegwayRmpDriverThread )

INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

FILE( GLOB srcs *.cpp )
GBX_ADD_LIBRARY( ${lib_name} DEFAULT ${srcs} )

#GBX_ADD_EXECUTABLE( SegwayRmpDriverTest segwayrmpdrivertest.cpp )
#TARGET_LINK_LIBRARIES( gbxsickacfrtest GbxSickAcfr )
