#
# Include and link against libOrcaIce and its dependencies
#
INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/utils
)

LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/utils/orcaice
    ${PROJECT_BINARY_DIR}/src/utils/orcaobj
)

LINK_LIBRARIES( OrcaIce OrcaObjects )
