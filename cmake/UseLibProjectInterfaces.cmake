#
# Setup include and link directories, special dependencies.
#

INCLUDE_DIRECTORIES( 
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
)

LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/orca
)

LINK_LIBRARIES( ${PROJECT_INTERFACE_LIB} )

# alexm: this tells the compiler to rebuild all slice-generated files if anyone of them
# is modified. I'm not sure why this stuff is here.
FILE( GLOB SRCS *.cpp )
FOREACH( SLICE_GENERATED_HEADER ${SLICE_GENERATED_HEADER_FILES} )
    SET_SOURCE_FILES_PROPERTIES( ${SRCS} PROPERTIES OBJECT_DEPENDS ${SLICE_GENERATED_HEADER_FILES} )
ENDFOREACH( SLICE_GENERATED_HEADER )
