OPTION( BUILD_ICEBOXSERVICE_EXAMPLE "Try to build an IceBox service example" ON )

IF ( BUILD_ICEBOXSERVICE_EXAMPLE )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/global_registry.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaIceBoxExample SHARED ${SRCS} )

ENDIF ( BUILD_ICEBOXSERVICE_EXAMPLE )
