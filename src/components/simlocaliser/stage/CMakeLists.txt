include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

file( GLOB srcs *.cpp )

# message( "fucking variables " ${PLAYER_INCLUDE_DIRS} )
include_directories( ${PLAYER_INCLUDE_DIRS} )
link_directories( ${PLAYER_LINK_DIRS} )
add_definitions( ${PLAYER_DEFINITIONS} )

set( lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LIBRARY( OrcaSimLocaliserStage DEFAULT ${lib_version} ${srcs} )
