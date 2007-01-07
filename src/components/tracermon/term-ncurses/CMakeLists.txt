INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# INCLUDE_DIRECTORIES(
#     ${PROJECT_SOURCE_DIR}/src/components/tracermon/termutils
#     )

# Source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaTracerMonTermNcurses SHARED ${SRCS} )

TARGET_LINK_LIBRARIES( OrcaTracerMonTermNcurses termutil )

INSTALL_TARGETS( /lib OrcaTracerMonTermNcurses )
