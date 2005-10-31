INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

ADD_EXECUTABLE( segwaytestmachine ${SRCS} )
