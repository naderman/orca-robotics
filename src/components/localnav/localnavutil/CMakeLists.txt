FILE( GLOB hdrs *.h )
FILE( GLOB srcs *.cpp )
GBX_ADD_LIBRARY( OrcaLocalNavUtil SHARED ${srcs} )

TARGET_LINK_LIBRARIES( OrcaLocalNavUtil HydroNavUtil )

GBX_ADD_HEADERS( localnavutil ${hdrs} )
