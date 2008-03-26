#
# Setup include and link directories, special dependencies.
#

INCLUDE_DIRECTORIES( 
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
)

LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/${PROJECT_NAME}
)

LINK_LIBRARIES( ${PROJECT_INTERFACE_LIB} )

# alexm: this tells the compiler to rebuild all slice-generated files if anyone of them
# is modified. I'm not sure why this stuff is here.
# FILE( GLOB srcs *.cpp )
# MESSAGE( STATUS "DEBUG: UseLibProjectInterfaces.cmake: srcs=${srcs}" )
# MESSAGE( STATUS "DEBUG: UseLibProjectInterfaces.cmake: ORCA_SLICE_GENERATED_HEADERS=${ORCA_SLICE_GENERATED_HEADERS}" )

# FOREACH( slice_generated_header ${ORCA_SLICE_GENERATED_HEADERS} )
#     SET_SOURCE_FILES_PROPERTIES( ${srcs} PROPERTIES OBJECT_DEPENDS ${slice_generated_header} )
# ENDFOREACH( slice_generated_header )
