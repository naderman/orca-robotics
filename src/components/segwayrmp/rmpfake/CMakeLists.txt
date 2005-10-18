INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( RmpFake ${SRCS} )
