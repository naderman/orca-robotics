INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# Source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaTracerMonTermIostream SHARED ${SRCS} )

TARGET_LINK_LIBRARIES( OrcaTracerMonTermIostream termutil )

INSTALL_TARGETS( /lib OrcaTracerMonTermIostream )
