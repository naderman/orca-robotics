SET( SICK_CORE_SRCS  laser serial time )

ADD_LIBRARY( OrcaLaser2dSickAcfr SHARED ${SICK_CORE_SRCS} sickacfrdriver )
TARGET_LINK_LIBRARIES( OrcaLaser2dSickAcfr sickutil)

INSTALL_TARGETS( /lib OrcaLaser2dSickAcfr )

INCLUDE_DIRECTORIES( 
    # for <config.h>
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src/components/laser2d/sickutil
    )
