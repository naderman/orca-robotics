IF ( NOT OS_LINUX )
  MESSAGE( STATUS "Not building OrcaLaser2dSickCarmen -- not Linux." )
ELSE ( NOT OS_LINUX )

  SET( SICK_CORE_SRCS sickdefines sick serial carmen_conversion )

  ADD_LIBRARY( OrcaLaser2dSickCarmen SHARED ${SICK_CORE_SRCS} sickcarmendriver )
  TARGET_LINK_LIBRARIES( OrcaLaser2dSickCarmen OrcaLaser2dUtil )

  INSTALL_TARGETS( /lib OrcaLaser2dSickCarmen )

  INCLUDE_DIRECTORIES( 
    # for <config.h>
    ${PROJECT_BINARY_DIR}
    )

  # Test for correct linking.
  LIST( REMOVE_ITEM SRCS 
    "${CMAKE_CURRENT_SOURCE_DIR}/linktest.cpp" )
  ADD_EXECUTABLE( linktest linktest.cpp )
  TARGET_LINK_LIBRARIES( linktest OrcaLaser2dSickCarmen )

ENDIF ( NOT OS_LINUX )