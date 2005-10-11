INCLUDE( ${PROJ_SOURCE_DIR}/config/xml_install.cmake )

OPTION( BUILD_CLIENT "Try to build a simple client" ON )

IF ( BUILD_CLIENT )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/global_registry.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_EXECUTABLE( client ${SRCS} )

ENDIF ( BUILD_CLIENT )
