IF ( NOT PLAYER_FOUND )
  MESSAGE( STATUS "Not building OrcaLaser2dPlayerClient -- Player not found." )
ELSE ( NOT PLAYER_FOUND )

  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

  FILE( GLOB SRCS *.cpp )
  LIST( REMOVE_ITEM SRCS 
    "${CMAKE_CURRENT_SOURCE_DIR}/linktest.cpp" )

  INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
  ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

  GLOBAL_ADD_LIBRARY( OrcaLaser2dPlayerClient SHARED ${SRCS} )
  TARGET_LINK_LIBRARIES( OrcaLaser2dPlayerClient ${PLAYER_LIBS} OrcaLaser2dUtil OrcaPlayer )

  # Test for correct linking.
  ADD_EXECUTABLE( linktest linktest.cpp )
  TARGET_LINK_LIBRARIES( linktest OrcaLaser2dPlayerClient )

ENDIF ( NOT PLAYER_FOUND )
