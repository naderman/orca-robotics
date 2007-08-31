INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# INCLUDE_DIRECTORIES(
#     ${PROJECT_SOURCE_DIR}/src/components/tracermon/termutil
#     )

FILE( GLOB SRCS *.cpp ../termutil/*.cpp )

GLOBAL_ADD_LIBRARY( OrcaTracerMonTermNcurses SHARED ${SRCS} )
TARGET_LINK_LIBRARIES( OrcaTracerMonTermNcurses ncurses form )
