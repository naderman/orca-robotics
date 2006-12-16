#
# Include and link against libOrcaIce and its dependencies
#
INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/libs
)

LINK_LIBRARIES( OrcaIce OrcaObjects )
