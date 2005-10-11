INCLUDE( ${PROJ_SOURCE_DIR}/config/xml_install.cmake )

OPTION( BUILD_DEVICE "Try to build sample device server" ON )

IF ( BUILD_DEVICE )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/global_registry.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_EXECUTABLE( device ${SRCS} )

ENDIF ( BUILD_DEVICE )
