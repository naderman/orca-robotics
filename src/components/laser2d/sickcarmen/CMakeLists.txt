SET( SICK_CORE_SRCS sickdefines sick serial carmen_conversion )

ADD_LIBRARY( OrcaLaser2dSickCarmen SHARED ${SICK_CORE_SRCS} sickcarmendriver )
TARGET_LINK_LIBRARIES( OrcaLaser2dSickCarmen sickutil )

INSTALL_TARGETS( /lib OrcaLaser2dSickCarmen )

INCLUDE_DIRECTORIES( 
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/components/laser2d/sickutil
  )
