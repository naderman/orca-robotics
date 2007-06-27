IF ( NOT OS_QNX )
  MESSAGE( STATUS "Not building OrcaLaser2dSickAcfr -- OS is not QNX" )
ELSE ( NOT OS_QNX )

  SET( SICK_CORE_SRCS laser )

  ADD_LIBRARY( OrcaLaser2dSickAcfr SHARED ${SICK_CORE_SRCS} sickacfrdriver )
  TARGET_LINK_LIBRARIES( OrcaLaser2dSickAcfr OrcaLaser2dUtil OrcaSerial )

  INSTALL_TARGETS( /lib OrcaLaser2dSickAcfr )

  INCLUDE_DIRECTORIES( 
    # for <confi$.h>
    ${PROJECT_BINARY_DIR}
    )

ENDIF ( NOT OS_QNX )
