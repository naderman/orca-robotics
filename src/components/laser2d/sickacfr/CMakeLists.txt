SET( SICK_CORE_SRCS laser time )

ADD_LIBRARY( OrcaLaser2dSickAcfr SHARED ${SICK_CORE_SRCS} sickacfrdriver )
TARGET_LINK_LIBRARIES( OrcaLaser2dSickAcfr sickutil OrcaSerial )

INSTALL_TARGETS( /lib OrcaLaser2dSickAcfr )

INCLUDE_DIRECTORIES( 
    # for <confi$.h>
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src/components/laser2d/sickutil
    )
